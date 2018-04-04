open Belt;

/* Filename -> Content */
type t = Map.String.t(string);

/*
 * A probe is a node used later to inject values into a node under the test.
 * Kind of terminal, but for tests.
 *
 * A probe stores reference to its target pin and that’s pin label is guaranted
 * to be normalized.
 */
module Probe = {
  type t = {
    node: Node.t,
    targetPin: Pin.t,
  };
  let getNode = (probe: t) => probe.node;
  let getTargetPin = (probe: t) => probe.targetPin;
  /*
   * Returns full patch path for the probe of a given type. The probe patch
   * nodes are stocked up in the `workspace` inside the package
   */
  let patchPath = (tp: Pin.dataType, dir: Pin.direction) : string =>
    "xod/tabtest/"
    ++ (
      switch (dir) {
      | Input => "inject-"
      | Output => "capture-"
      }
    )
    ++ (
      switch (tp) {
      | Pulse => "pulse"
      | Boolean => "boolean"
      | Number => "number"
      | String => "string"
      }
    );
  /*
   * Creates a new probe node matching the type of pin provided
   */
  let create = pin => {
    node: Node.create(patchPath(Pin.getType(pin), Pin.getDirection(pin))),
    targetPin: pin,
  };
  /*
   * Returns a key of the only pin conventionally labeled `VAL` for a
   * probe node.
   */
  let getPinKeyExn = (probe, project) => {
    let node = getNode(probe);
    let pt = Node.getType(node);
    let patch =
      switch (Project.getPatchByNode(project, node)) {
      | Some(patch') => patch'
      | None => Js.Exn.raiseError("Probe has unexpected type " ++ pt)
      };
    let pin =
      Patch.findPinByLabel(patch, "VAL", ~normalize=true, ~direction=None);
    switch (pin) {
    | Some(pin) => Pin.getKey(pin)
    | None =>
      Js.Exn.raiseError(
        "Expected all probes to have the only pin labeled 'VAL'. "
        ++ pt
        ++ " violates the rule",
      )
    };
  };
};

module Probes = {
  type t = list(Probe.t);
  let map = List.map;
  let keepToPinDirection = (probes, dir) =>
    List.keep(probes, probe =>
      probe |. Probe.getTargetPin |. Pin.getDirection == dir
    );
  let keepInjecting = keepToPinDirection(_, Input);
  let keepCapturing = keepToPinDirection(_, Output);
};

/* TODO: smarter errors */
let newError = (message: string) : Js.Exn.t =>
  try (Js.Exn.raiseError(message)) {
  | Js.Exn.Error(e) => e
  };

/*
 * Test bench is a patch containing a central node under the test and
 * a set of probes connected to each of its pins.
 */
module Bench = {
  type t = {
    patch: Patch.t,
    /* Maps node ids to symbolic names.
          For probes they like "probe_XYZ", where XYZ is the label of
          corresponding input or output. The central node maps to "theNode".
       */
    symbolMap: Map.String.t(string),
    probes: Probes.t,
  };
  /*
   * Creates a new bench for the project provided with the specified
   * node instance to test. The bench node is *not* associated to the
   * project automatically.
   */
  let create = (project, patchPathToTest) : Js.Result.t(t, Js.Exn.t) => {
    let pptt = patchPathToTest;
    let theNode = Node.create(pptt);
    let theNodeId = Node.getId(theNode);
    let draftBench: t = {
      patch: Patch.create() |. Patch.assocNode(theNode),
      symbolMap: Map.String.empty |. Map.String.set(theNodeId, "theNode"),
      probes: [],
    };
    switch (Project.getPatchByPath(project, pptt)) {
    | None => Error(newError({j|Patch $pptt not found|j}))
    | Some(patchToTest) =>
      Ok(
        Patch.listPins(patchToTest)
        |. Pin.normalizeLabels
        /*
            For each pin of a node under the test, create a new probe node
            and link its `VAL` to that pin.
         */
        |. List.map(Probe.create)
        |. List.reduce(
             draftBench,
             (bench, probe) => {
               let probeNode = Probe.getNode(probe);
               let probeId = Node.getId(probeNode);
               let probePK = Probe.getPinKeyExn(probe, project);
               let targPin = Probe.getTargetPin(probe);
               let targPK = Pin.getKey(targPin);
               let link =
                 switch (Pin.getDirection(targPin)) {
                 | Input =>
                   Link.create(
                     ~fromPin=probePK,
                     ~fromNode=probeId,
                     ~toPin=targPK,
                     ~toNode=theNodeId,
                   )
                 | Output =>
                   Link.create(
                     ~fromPin=targPK,
                     ~fromNode=theNodeId,
                     ~toPin=probePK,
                     ~toNode=probeId,
                   )
                 };
               {
                 patch:
                   bench.patch
                   |. Patch.assocNode(probeNode)
                   |. Patch.assocLinkExn(link),
                 symbolMap:
                   bench.symbolMap
                   |. Map.String.set(
                        probeId,
                        "probe_" ++ Pin.getLabel(targPin),
                      ),
                 probes: [probe, ...bench.probes],
               };
             },
           ) /* reduce */
      ) /* OK */
    };
  };
};

module TabData = {
  type record = Map.String.t(string);
  type t = list(record);
  let forPatch = patch : Resulty.t(t, Js.Exn.t) =>
    Ok([
      Map.String.empty
      |. Map.String.set("COND", "true")
      |. Map.String.set("T", "42")
      |. Map.String.set("F", "51")
      |. Map.String.set("R", "42"),
      Map.String.empty
      |. Map.String.set("COND", "false")
      |. Map.String.set("R", "51"),
    ]);
  let map = List.map;
};

module TestCase = {
  type t = string;
  let generate =
      (tabData: TabData.t, idMap: Map.String.t(string), probes: Probes.t)
      : t => {
    let nodeAliases =
      idMap
      |. Map.String.toList
      |. List.map(((name, id)) => {j|auto& $name = xod::node_$id;|j});
    let case = record => {
      let injectionStatements =
        probes
        |. Probes.keepInjecting
        |. Probes.map(probe => {
             let name = probe |. Probe.getTargetPin |. Pin.getLabel;
             switch (record |. Map.String.get(name)) {
             | Some(value) => {j|INJECT(probe_$name, $value);|j}
             | None => {j|// No changes for $name|j}
             };
           });
      let assertionsStatements =
        probes
        |. Probes.keepCapturing
        |. Probes.map(probe => {
             let name = probe |. Probe.getTargetPin |. Pin.getLabel;
             switch (record |. Map.String.get(name)) {
             | Some(value) =>
               Cpp.requireEqual(
                 ~expect=value,
                 ~actual={j|probe_$name.state.lastValue|j},
                 ~children=[],
                 (),
               )
             | None => {j|// no expectation for $name|j}
             };
           });
      Cpp.source(
        ~children=
          List.flatten([
            [""],
            injectionStatements,
            ["loop();"],
            assertionsStatements,
          ]),
        (),
      );
    };
    let cases = tabData |. TabData.map(case);
    Cpp.(
      <source>
        "#include \"catch.hpp\""
        <blank />
        <source> ...nodeAliases </source>
        <blank />
        "#define INJECT(probe, value) { \\"
        "        (probe).output_VAL = (value); \\"
        "        (probe).isNodeDirty = true; \\"
        "    }"
        <blank />
        <testCase name="xod/core/if-else">
          "setup();"
          "loop();"
          <source> ...cases </source>
        </testCase>
      </source>
    );
  };
};

let generateSuite = (project, patchPath) : Resulty.t(t, Js.Exn.t) => {
  let benchPatchPath = "@/tabtest-bench";
  let benchR = Bench.create(project, patchPath);
  let probesR = Resulty.map(benchR, bench => bench.probes);
  let projectWithBenchR =
    Resulty.flatMap(benchR, bench =>
      Project.assocPatch(project, benchPatchPath, bench.patch)
    );
  let programR =
    Resulty.flatMap(
      projectWithBenchR,
      Transpiler.transpile(_, benchPatchPath),
    );
  let tabDataR =
    Resulty.flatMap(benchR, bench => TabData.forPatch(bench.patch));
  let idMapR: Resulty.t(Map.String.t(string), Js.Exn.t) =
    Ok(
      Map.String.empty
      |. Map.String.set("probe_COND", "0")
      |. Map.String.set("probe_T", "1")
      |. Map.String.set("probe_F", "2")
      |. Map.String.set("probe_R", "4"),
    );
  let testCaseR = Resulty.lift3(TestCase.generate, tabDataR, idMapR, probesR);
  Resulty.lift2(
    (program: Transpiler.program, testCase) =>
      Map.String.empty
      |. Map.String.set("sketch.cpp", program.code)
      |. Map.String.set("test.inl", testCase),
    programR,
    testCaseR,
  );
};

open Belt;

type t = Map.String.t(string);

/*
 * A probe is a node used later to inject values into a node under the test.
 * Kind of terminal, but for tests.
 */
module Probe = {
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
  let create = pin =>
    Node.create(patchPath(Pin.getType(pin), Pin.getDirection(pin)));
  /*
   * Returns a key of the only pin conventionally labeled `VAL` for a
   * probe node.
   */
  let getPinKeyExn = (node, project) => {
    let pt = Node.getType(node);
    let patch =
      switch (Project.getPatchByNode(project, node)) {
      | Some(patch') => patch'
      | None => Js.Exn.raiseError("Probe has unexpected type " ++ pt)
      };
    let pin =
      Patch.findPinByLabel(patch, "VAL", ~normalize=true, ~direction=None);
    switch (pin) {
    | Some(pin) => pin |. Pin.getKey
    | None =>
      Js.Exn.raiseError(
        "Expected all probes to have the only pin labeled 'VAL'. "
        ++ pt
        ++ " violates the rule",
      )
    };
  };
};

/* TODO: smarter errors */
let newError = (_message: string) : Js.Exn.t => [%bs.raw "new Error()"];

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
        |. List.map(pin => (pin, pin |> Probe.create))
        |. List.reduce(
             draftBench,
             (bench, (targPin, probe)) => {
               let probeId = Node.getId(probe);
               let probePK = Probe.getPinKeyExn(probe, project);
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
                   |. Patch.assocNode(probe)
                   |. Patch.assocLinkExn(link),
                 symbolMap:
                   bench.symbolMap
                   |. Map.String.set(
                        probeId,
                        "probe_" ++ Pin.getLabel(targPin),
                      ),
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
};

module TestCase = {
  type t = string;
  let generate = (tabData: TabData.t, idMap: Map.String.t(string)) : t =>
    Cpp.(
      <source>
        "#include \"catch.hpp\""
        "#include \"Arduino.h\""
        <blank />
        "auto& probe_COND = xod::node_0;"
        <blank />
        <funcDef name="runCase">
          "theNode.isNodeDirty = true;"
          "loop();"
        </funcDef>
        <blank />
        <testCase name="xod/core/if-else">
          "setup();"
          "loop();"
          <blank />
          "using namespace xod;"
          <blank />
          "// Case"
          "runCase();"
          <requireEqual actual="node_3.output_R" expect="0" />
        </testCase>
      </source>
    );
};

let generateSuite = (project, patchPath) : Resulty.t(t, Js.Exn.t) => {
  let benchPatchPath = "@/tabtest-bench";
  let benchR = Bench.create(project, patchPath);
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
    Ok(Map.String.empty);
  let testCaseR = Resulty.lift2(TestCase.generate, tabDataR, idMapR);
  Resulty.lift2(
    (program: Transpiler.program, testCase) =>
      Map.String.empty
      |. Map.String.set("sketch.cpp", program.code)
      |. Map.String.set("test.inl", testCase),
    programR,
    testCaseR,
  );
};

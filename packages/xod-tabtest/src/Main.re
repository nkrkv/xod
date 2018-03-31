open Belt;

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

/* TODO: create a new project rather than loading the `blink` */
Loader.loadProject(["../../workspace", "workspace"], "../../workspace/blink")
|> Js.Promise.then_(project => {
     let patchPathToTest = "xod/core/if-else";
     Bench.create(project, patchPathToTest)
     |. Resulty.map((suite: Bench.t) => {
          Js.log("Symbol map");
          Js.log(suite.symbolMap |> Map.String.toArray);
          suite.patch;
        })
     |. Resulty.flatMap(Project.assocPatch(project, "@/test"))
     |. Resulty.flatMap(Transpiler.transpile(_, "@/test"))
     |> Js.Promise.resolve;
   })
|> Js.Promise.then_((result: Js.Result.t(Transpiler.program, Js.Exn.t)) =>
     switch (result) {
     | Js.Result.Ok(program) =>
       Js.log("OK");
       Js.log(program.code);
       Js.log("***");
       Js.log(program.nodeIdMap |> Map.String.toArray);
       Js.Promise.resolve("OK");
     | Js.Result.Error(_) =>
       Js.log("Error");
       Js.Promise.resolve("Error");
     }
   )
|> Js.Promise.catch(err => {
     Js.log(err);
     Js.Promise.resolve("Error");
   });

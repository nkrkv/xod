/*
 * A probe is a node used later to inject values into a node under the test.
 * Kind of terminal, but for tests.
 */
module Probe = {
  /*
   * Returns full patch path for the probe of a given type. The probe patch
   * nodes are stocked up in the `workspace` inside the package
   */
  let typeToPatchPath = (tp: Pin.dataType) : string =>
    "xod/tabtest/"
    ++ (
      switch (tp) {
      | Pulse => "probe-pulse"
      | Boolean => "probe-boolean"
      | Number => "probe-number"
      | String => "probe-string"
      }
    );
  /*
   * Creates a new probe node matching the type of pin provided
   */
  let create = pin =>
    Node.create(Node.origin, pin |> Pin.getType |> typeToPatchPath);
  /*
   * Returns a pin key of the only output conventionally labeled `VAL` for a
   * probe node.
   */
  let getPinKeyExn = (node, project) => {
    let pt = Node.getType(node);
    let patch =
      switch (Project.getPatchByNode(node, project)) {
      | Some(patch') => patch'
      | None => Js.Exn.raiseError("Probe has unexpected type " ++ pt)
      };
    let pin =
      Patch.findPinByLabel(
        patch,
        "VAL",
        ~normalize=true,
        ~direction=Some(Pin.Output),
      );
    switch (pin) {
    | Some(pin) => pin |> Pin.getKey
    | None =>
      Js.Exn.raiseError(
        "Expected all probes to have the only output labeled 'VAL'. "
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
         corresponding input.  The central node maps to "theNode".
       */
    symbolMap: Belt.Map.String.t(string),
  };
  /*
   * Creates a new bench for the project provided with the specified
   * node instance to test. The bench node is *not* associated to the
   * project automatically.
   */
  let create = (project, patchPathToTest) : Js.Result.t(t, Js.Exn.t) => {
    let pptt = patchPathToTest;
    let theNode = Node.create(Node.origin, pptt);
    let theNodeId = Node.getId(theNode);
    let draftBench: t = {
      patch: Patch.create() |> Patch.assocNode(theNode),
      symbolMap:
        Belt.Map.String.empty |> Belt.Map.String.set(_, theNodeId, "theNode"),
    };
    switch (project |> Project.getPatchByPath(pptt)) {
    | None => Error(newError({j|Patch $pptt not found|j}))
    | Some(patchToTest) =>
      Ok(
        Patch.listInputPins(patchToTest)
        |> Pin.normalizeLabels
        /*
            For each input pin of a node under the test, create a new probe node
            and link its output `VAL` to that pin.
         */
        |> Belt.List.map(_, pin => (pin, pin |> Probe.create))
        |> Belt.List.reduce(
             _,
             draftBench,
             (bench, (targPin, probe)) => {
               let probeId = Node.getId(probe);
               let link =
                 Link.create(
                   Pin.getKey(targPin),
                   theNodeId,
                   Probe.getPinKeyExn(probe, project),
                   probeId,
                 );
               {
                 patch:
                   bench.patch
                   |> Patch.assocNode(probe)
                   |> Patch.assocLinkExn(link),
                 symbolMap:
                   bench.symbolMap
                   |> Belt.Map.String.set(
                        _,
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
     |> Resulty.map((suite: Bench.t) => {
          Js.log("Symbol map");
          Js.log(suite.symbolMap |> Belt.Map.String.toArray);
          suite.patch;
        })
     |> Resulty.chain(Project.assocPatch("@/test", _, project))
     |> Resulty.chain(Transpiler.transpile(_, "@/test"))
     |> Js.Promise.resolve;
   })
|> Js.Promise.then_((result: Js.Result.t(Transpiler.program, Js.Exn.t)) =>
     switch (result) {
     | Js.Result.Ok(program) =>
       Js.log("OK");
       Js.log(program.code);
       Js.log("***");
       Js.log(program.nodeIdMap |> Belt.Map.String.toArray);
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

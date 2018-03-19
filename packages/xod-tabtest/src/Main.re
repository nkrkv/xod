let typeToProbeName = (tp: Pin.dataType) : string =>
  switch (tp) {
  | Pulse => "probe-pulse"
  | Boolean => "probe-boolean"
  | Number => "probe-number"
  | String => "probe-string"
  };

let createProbe = pin =>
  Node.create(
    Node.origin,
    "xod/tabtest/" ++ (pin |> Pin.getType |> typeToProbeName),
  );

let getProbePinKeyExn = (node, project) => {
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

/* TODO: smarter errors */
let newError = (_message: string) : Js.Exn.t => [%bs.raw "new Error()"];

let createTestPatch =
    (project, patchPathToTest)
    : Js.Result.t(Patch.t, Js.Exn.t) => {
  let pptt = patchPathToTest;
  let theNode = Node.create(Node.origin, pptt);
  let theNodeId = Node.getId(theNode);
  let draftPatch = Patch.create() |> Patch.assocNode(theNode);
  switch (project |> Project.getPatchByPath(pptt)) {
  | None => Error(newError({j|Patch $pptt not found|j}))
  | Some(patchToTest) =>
    Patch.listInputPins(patchToTest)
    |> Pin.normalizeLabels
    /*
       For each input pin of a node under the test, create a new probe node
       and link its output `VAL` to that pin.
    */
    |> Belt.List.map(_, pin => (pin, pin |> createProbe))
    |> Belt.List.reduce(
         _, Js.Result.Ok(draftPatch), (patchAcc, (targPin, probe)) =>
         patchAcc
         |> Resulty.chain(patch => {
              let link =
                Link.create(
                  Pin.getKey(targPin),
                  theNodeId,
                  getProbePinKeyExn(probe, project),
                  Node.getId(probe),
                );
              patch |> Patch.assocNode(probe) |> Patch.assocLink(link);
            })
       )
  };
};

/* TODO: create a new project rather than loading the `blink` */
Loader.loadProject(["../../workspace", "workspace"], "../../workspace/blink")
|> Js.Promise.then_(project => {
     let patchPathToTest = "xod/core/if-else";
     createTestPatch(project, patchPathToTest)
     |> Resulty.chain(Project.assocPatch("@/test", _, project))
     |> Resulty.chain(Transpiler.transpile(_, "@/test"))
     |> Js.Promise.resolve;
   })
|> Js.Promise.then_(result =>
     switch (result) {
     | Js.Result.Ok(code) =>
       Js.log(code);
       /*Js.log("OK");*/
       Js.Promise.resolve(code);
     | Js.Result.Error(_) =>
       Js.log("Error");
       Js.Promise.resolve("Error");
     }
   )
|> Js.Promise.catch(err => {
     Js.log(err);
     Js.Promise.resolve("Error");
   });

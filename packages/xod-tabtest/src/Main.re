let createTestInputNode = _pin =>
  Node.create(Node.origin, "xod/tabtest/input-number");

/* TODO: smarter errors */
let newError = (_message: string) : Js.Exn.t => [%bs.raw "new Error()"];

let createTestPatch =
    (project: Project.t, patchPatchToTest: PatchPath.t)
    : Js.Result.t(Patch.t, Js.Exn.t) => {
  let pptt = patchPatchToTest;
  let theNode = Node.create(Node.origin, pptt);
  let thePatch = ref(Patch.create() |> Patch.assocNode(theNode));
  switch (project |> Project.getPatchByPath(pptt)) {
  | None => Error(newError({j|Patch $pptt not found|j}))
  | Some(patchToTest) =>
    Patch.listInputPins(patchToTest)
    |> Pin.normalizeLabels
    |> Belt.List.map(_, createTestInputNode)
    |> Belt.List.forEach(_, node =>
         thePatch := thePatch^ |> Patch.assocNode(node)
       );
    Ok(thePatch^);
  };
};

/* TODO: create a new project rather than loading the `blink` */
Loader.loadProject(["../../workspace", "workspace"], "../../workspace/blink")
|> Js.Promise.then_(project => {
     let patchPathToTest = "xod/core/not";
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
   );

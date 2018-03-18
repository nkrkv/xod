let createTestPatch = (nodeToTest: PatchPath.t) : Patch.t => {
  let node = Node.create(Node.origin, nodeToTest);
  Patch.create() |> Patch.assocNode(node);
};

/* TODO: create a new project rather than loading the `blink` */
Loader.loadProject(["../../workspace"], "../../workspace/blink")
|> Js.Promise.then_(project => {
     let patchPathToTest = "xod/core/not";
     (
       switch (project |> Project.getPatchByPath(patchPathToTest)) {
       | Some(patch) =>
         Js.log("Inputs:");
         Js.log(Patch.listInputPins(patch) |> Pin.normalizeLabels);
         Js.log("Outputs:");
         Js.log(Patch.listOutputPins(patch) |> Pin.normalizeLabels);
         true;
       | None =>
         Js.log("Patch path not found");
         false;
       }
     )
     |> ignore;
     createTestPatch(patchPathToTest)
     |> Project.assocPatch("@/test", _, project)
     |> Resulty.chain(Transpiler.transpile(_, "@/test"))
     |> Js.Promise.resolve;
   })
|> Js.Promise.then_(result =>
     switch (result) {
     | Js.Result.Ok(code) =>
       /*Js.log(code);*/
       Js.log("OK");
       Js.Promise.resolve(code);
     | Js.Result.Error(_) =>
       Js.log("Error");
       Js.Promise.resolve("Error");
     }
   );

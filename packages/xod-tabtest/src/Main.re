let createTestPatch = (nodeToTest: PatchPath.t) : Patch.t => {
  let node = Node.create(Node.origin, nodeToTest);
  Patch.create() |> Patch.assocNode(node);
};

/* TODO: create a new project rather than loading the `blink` */
Loader.loadProject(["../../workspace"], "../../workspace/blink")
|> Js.Promise.then_(project =>
     createTestPatch("xod/core/not")
     |> Project.assocPatch("@/test", _, project)
     |> (
       res =>
         (
           switch (res) {
           | Ok(project') =>
             /* TODO: chain result */
             switch (Transpiler.transpile(project', "@/test")) {
             | Ok(code) => Js.Result.Ok(code)
             | Error(err) => Js.Result.Error(err)
             }
           | Error(err) => Js.Result.Error(err)
           }
         )
         |> Js.Promise.resolve
     )
   )
|> Js.Promise.then_(result =>
     switch (result) {
     | Js.Result.Ok(code) =>
       Js.log(code);
       Js.Promise.resolve(code);
     | Js.Result.Error(_) =>
       Js.log("Error");
       Js.Promise.resolve("Error");
     }
   );

Loader.loadProject(["../../workspace"], "../../workspace/blink")
|> Js.Promise.then_(project => {
     Js.log(project);
     Js.Promise.resolve(Transpiler.transpile(project, "@/main"));
   })
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

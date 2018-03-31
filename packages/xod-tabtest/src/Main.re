open Belt;

module Promise = Js.Promise;

/* TODO: create a new project rather than loading the `blink` */
Loader.loadProject(["../../workspace", "workspace"], "../../workspace/blink")
|> Promise.then_(proj =>
     proj |. Tabtest.generateSuite("xod/core/if-else") |. Promise.resolve
   )
|> Promise.then_((result: Resulty.t(Tabtest.t, Js.Exn.t)) =>
     switch (result) {
     | Js.Result.Ok(suite) =>
       Js.log(suite.sourceFiles |. Map.String.toArray);
       Js.log("***");
       Js.log(suite.runCommand |. List.toArray);
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

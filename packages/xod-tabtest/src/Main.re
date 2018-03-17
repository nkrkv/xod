Loader.loadProjectWithLibs(
  ["../../workspace"],
  "../../workspace/__lib__/xod/core",
)
|> Js.Promise.then_(project => {
     Js.log(project);
     Js.Promise.resolve(project);
   });

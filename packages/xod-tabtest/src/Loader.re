[@bs.module "xod-fs"]
external loadProjectWithLibsJs :
  (array(string), string) => Js.Promise.t(Project.t) =
  "loadProjectWithLibs";

let loadProjectWithLibs = (workspaces, patchPath) =>
  loadProjectWithLibsJs(Belt.List.toArray(workspaces), patchPath);

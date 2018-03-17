module FFI = {
  [@bs.module "xod-fs"]
  external loadProject : (array(string), string) => Js.Promise.t(Project.t) =
    "loadProject";
};

let loadProject = (workspaces, patchPath) =>
  FFI.loadProject(Belt.List.toArray(workspaces), patchPath);

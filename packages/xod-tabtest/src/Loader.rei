/**
  Loads a project from the given list of workspaces and a path to the project
  */
let loadProjectWithLibs: (list(string), string) => Js.Promise.t(Project.t);

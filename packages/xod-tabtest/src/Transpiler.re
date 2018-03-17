module TProject = {
  type t;
};

module Js = {
  [@bs.module "xod-arduino"]
  external transformProject :
    (Project.t, string) => Either.t(Js.Exn.t, TProject.t) =
    "transformProject";
  [@bs.module "xod-arduino"]
  external transpile : TProject.t => string = "transpile";
};

let transpile = (project, patchPath) =>
  Js.transformProject(project, patchPath)
  |> Either.toResult
  |> Resulty.map(Js.transpile);

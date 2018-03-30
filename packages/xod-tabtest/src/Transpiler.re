module TProject = {
  type t;
};

type program = {
  code: string,
  nodeIdMap: Belt.Map.String.t(string),
};

[@bs.module "xod-arduino"]
external _transformProject :
  (Project.t, string) => Either.t(Js.Exn.t, TProject.t) =
  "transformProject";

[@bs.module "xod-arduino"]
external _transpile : TProject.t => string = "transpile";

[@bs.module "xod-arduino"]
external _getNodeIdsMap : TProject.t => Js.Dict.t(string) = "getNodeIdsMap";

let transpile = (project, patchPath) : Js.Result.t(program, Js.Exn.t) =>
  _transformProject(project, patchPath)
  |> Either.toResult
  |> Resulty.map(tProject =>
       {
         code: _transpile(tProject),
         nodeIdMap:
           _getNodeIdsMap(tProject)
           |> Js.Dict.entries
           |> Belt.Map.String.fromArray,
       }
     );

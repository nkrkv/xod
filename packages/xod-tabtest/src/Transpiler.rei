type program = {
  code: string,
  nodeIdMap: Belt.Map.String.t(string),
};

let transpile: (Project.t, PatchPath.t) => Js.Result.t(program, Js.Exn.t);

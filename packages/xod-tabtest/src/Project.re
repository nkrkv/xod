type t = Js.Types.obj_val;

module FFI = {
  [@bs.module "xod-project"]
  external assocPatch : (PatchPath.t, Patch.t, t) => Either.t(Js.Exn.t, t) =
    "assocPatch";
  [@bs.module "xod-project"]
  external getPatchByPath : (PatchPath.t, t) => Maybe.t(Patch.t) =
    "getPatchByPath";
};

let assocPatch = (path, patch, project) =>
  FFI.assocPatch(path, patch, project) |> Either.toResult;

let getPatchByPath = (path, project) =>
  FFI.getPatchByPath(path, project) |> Maybe.toOption;

let getPatchByNode = (node, project) =>
  getPatchByPath(node |> Node.getType, project);

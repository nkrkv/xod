type t = Js.Types.obj_val;

module FFI = {
  [@bs.module "xod-project"]
  external assocPatch : (PatchPath.t, Patch.t, t) => Either.t(Js.Exn.t, t) =
    "assocPatch";
};

let assocPatch = (path, patch, project) =>
  FFI.assocPatch(path, patch, project) |> Either.toResult;

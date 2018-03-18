type t;

let assocPatch: (PatchPath.t, Patch.t, t) => Js.Result.t(t, Js.Exn.t);

let getPatchByPath: (PatchPath.t, t) => option(Patch.t);

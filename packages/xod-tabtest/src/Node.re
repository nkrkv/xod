type t = Js.Types.obj_val;

type position = {
  .
  "x": int,
  "y": int,
};

let origin = {"x": 0, "y": 0};

[@bs.module "xod-project"]
external create : (position, PatchPath.t) => t = "createNode";

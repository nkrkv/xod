type t = Js.Types.obj_val;

type id = string;

type position = {
  .
  "x": int,
  "y": int,
};

let origin = {"x": 0, "y": 0};

[@bs.module "xod-project"]
external create : (position, PatchPath.t) => t = "createNode";

[@bs.module "xod-project"] external getId : t => id = "getNodeId";

[@bs.module "xod-project"] external getType : t => PatchPath.t = "getNodeType";

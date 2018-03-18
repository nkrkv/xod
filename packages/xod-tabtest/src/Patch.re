type t = Js.Types.obj_val;

[@bs.module "xod-project"] external create : unit => t = "createPatch";

[@bs.module "xod-project"] external assocNode : (Node.t, t) => t = "assocNode";

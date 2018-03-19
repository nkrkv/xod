type t = Js.Types.obj_val;

[@bs.module "xod-project"]
external create : (Pin.key, Node.id, Pin.key, Node.id) => t = "createLink";

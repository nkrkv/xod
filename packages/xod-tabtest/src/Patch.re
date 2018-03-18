type t = Js.Types.obj_val;

module FFI = {
  [@bs.module "xod-project"]
  external listPins : t => array(Pin.t) = "listPins";
};

[@bs.module "xod-project"] external create : unit => t = "createPatch";

[@bs.module "xod-project"] external assocNode : (Node.t, t) => t = "assocNode";

let listPins = patch => FFI.listPins(patch) |> Belt.List.ofArray;

let listInputPins = patch =>
  patch
  |> listPins
  |> Belt.List.keep(_, pin => Pin.getDirection(pin) == Pin.Input);

let listOutputPins = patch =>
  patch
  |> listPins
  |> Belt.List.keep(_, pin => Pin.getDirection(pin) == Pin.Output);

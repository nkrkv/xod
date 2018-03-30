open Belt;

type t = Js.Types.obj_val;

module FFI = {
  [@bs.module "xod-project"]
  external listPins : t => array(Pin.t) = "listPins";
};

[@bs.module "xod-project"] external create : unit => t = "createPatch";

[@bs.module "xod-project"] external assocNode : (Node.t, t) => t = "assocNode";

[@bs.module "xod-project"]
external _assocLink : (Link.t, t) => Either.t(Js.Exn.t, t) = "assocLink";

let assocLink = (link, patch) => _assocLink(link, patch) |> Either.toResult;

let assocLinkExn = (link, patch) =>
  switch (assocLink(link, patch)) {
  | Ok(patch') => patch'
  | Error(err) =>
    Js.Exn.raiseError(err |> Js.Exn.message |> Js.Option.getWithDefault(""))
  };

let listPins = patch => FFI.listPins(patch) |> List.fromArray;

let listInputPins = patch =>
  patch |> listPins |> List.keep(_, pin => Pin.getDirection(pin) == Pin.Input);

let listOutputPins = patch =>
  patch
  |> listPins
  |> List.keep(_, pin => Pin.getDirection(pin) == Pin.Output);

/* TODO: is it defined anywhere already? */
let identity = a => a;

let findPinByLabel = (patch, label, ~normalize, ~direction) : option(Pin.t) =>
  listPins(patch)
  |> (normalize ? Pin.normalizeLabels : identity)
  |> List.keep(_, pin => Pin.getLabel(pin) == label)
  |> (
    pins =>
      switch (direction) {
      | None => pins
      | Some(dir) => List.keep(pins, pin => Pin.getDirection(pin) == dir)
      }
  )
  |> (
    pins =>
      switch (pins) {
      | [onlyPin] => Some(onlyPin)
      | _ => None
      }
  );
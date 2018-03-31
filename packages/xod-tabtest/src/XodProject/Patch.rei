type t;

let create: unit => t;

let assocNode: (Node.t, t) => t;

let assocLink: (Link.t, t) => Js.Result.t(t, Js.Exn.t);

let assocLinkExn: (Link.t, t) => t;

let listPins: t => list(Pin.t);

let listInputPins: t => list(Pin.t);

let listOutputPins: t => list(Pin.t);

let findPinByLabel:
  (t, string, ~normalize: bool, ~direction: option(Pin.direction)) =>
  option(Pin.t);

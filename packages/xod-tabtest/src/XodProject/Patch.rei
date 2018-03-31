type t;

let create: unit => t;

let assocNode: (t, Node.t) => t;

let assocLink: (t, Link.t) => Js.Result.t(t, Js.Exn.t);

let assocLinkExn: (t, Link.t) => t;

let listPins: t => list(Pin.t);

let listInputPins: t => list(Pin.t);

let listOutputPins: t => list(Pin.t);

let findPinByLabel:
  (t, string, ~normalize: bool, ~direction: option(Pin.direction)) =>
  option(Pin.t);

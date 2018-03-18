type t;

let create: unit => t;

let assocNode: (Node.t, t) => t;

let listPins: t => list(Pin.t);

let listInputPins: t => list(Pin.t);

let listOutputPins: t => list(Pin.t);

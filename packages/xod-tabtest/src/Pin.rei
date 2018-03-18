type t;

type direction =
  | Input
  | Output;

let getDirection: t => direction;

let normalizeLabels: list(t) => list(t);

type t;

type position = {
  .
  "x": int,
  "y": int,
};

let origin: position;

let create: (position, PatchPath.t) => t;

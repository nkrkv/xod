type t;

type id = string;

/* TODO: encapsulate in record */
type position = {
  .
  "x": int,
  "y": int,
};

/* TODO: hide, remove from `create` */
let origin: position;

let create: (position, PatchPath.t) => t;

let getId: t => id;

let getType: t => PatchPath.t;

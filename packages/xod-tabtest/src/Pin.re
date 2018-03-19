type t = {
  .
  "key": string,
  "direction": string,
  "label": string,
  "_type": string,
  "defaultValue": string,
  "order": int,
  "description": string,
  "isBindable": bool,
};

type dataType =
  | Pulse
  | Boolean
  | Number
  | String;

type direction =
  | Input
  | Output;

module FFI = {
  [@bs.module "xod-project"]
  external normalizeLabels : array(t) => array(t) = "normalizePinLabels";
};

let getDirection = (pin: t) : direction => {
  let dir = pin##direction;
  switch (dir) {
  | "input" => Input
  | "output" => Output
  | _ =>
    Js.Exn.raiseTypeError(
      {j|Pin direction should be "input" or "output", got "$dir"|j},
    )
  };
};

let normalizeLabels = pins =>
  pins |> Belt.List.toArray |> FFI.normalizeLabels |> Belt.List.ofArray;

let getType = (pin: t) : dataType => {
  let tp = pin##_type;
  switch (tp) {
  | "pulse" => Pulse
  | "boolean" => Boolean
  | "number" => Number
  | "string" => String
  | _ =>
    Js.Exn.raiseTypeError(
      {j|Pin type should be "pulse", "boolean", "number", etc, got "$tp"|j},
    )
  };
};

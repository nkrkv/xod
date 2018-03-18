type t = {
  .
  "key": string,
  "direction": string,
  "label": string,
  "type": string,
  "defaultValue": string,
  "order": int,
  "description": string,
  "isBindable": bool,
};

type direction =
  | Input
  | Output;

let getDirection = (pin: t) : direction => {
  let dir = pin##direction;
  switch (pin##direction) {
  | "input" => Input
  | "output" => Output
  | _ =>
    Js.Exn.raiseTypeError(
      {j|Pin direction should be "input" or "output", got "$dir"|j},
    )
  };
};

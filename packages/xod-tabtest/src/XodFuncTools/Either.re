type t('left, 'right) = Js.Types.obj_val;

module FFI = {
  [@bs.module "xod-func-tools"]
  external foldEither : ('left => 'a, 'right => 'a, t('left, 'right)) => 'a =
    "foldEither";
};

let toResult = either =>
  either
  |> FFI.foldEither(
       left => Js.Result.Error(left),
       right => Js.Result.Ok(right),
     );

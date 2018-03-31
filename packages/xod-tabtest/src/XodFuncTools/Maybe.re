type t('a) = Js.Types.obj_val;

module FFI = {
  [@bs.module "xod-func-tools"]
  external foldMaybe : ('b, 'a => 'b, t('a)) => 'b = "foldMaybe";
};

let toOption = maybe =>
  maybe |> FFI.foldMaybe(None, justValue => Some(justValue));

open Belt;

let _join = (strings: list(string), delimiter: string) =>
  List.reduce(strings, ("", ""), ((acc, delimiter'), s) =>
    (acc ++ delimiter' ++ s, delimiter)
  );

let join = (strings, delimiter) => {
  let (str, _) = _join(strings, delimiter);
  str;
};

let joinLines = strings => join(strings, "\n");

let indent = str => str |> Js.String.replaceByRe([%bs.re "/^/gm"], "    ");

let enquote = x => {j|"$x"|j};

/* ↓ C++ JSX ↓ */
let source = (~children, ()) => join(children, "\n");

let blank = (~children, ()) => "";

let indented = (~children, ()) => children |. joinLines |. indent;

let block = (~children, ()) =>
  ["{", indented(~children, ()), "}"] |. joinLines;

let funcDef = (~name, ~children, ()) =>
  "void " ++ name ++ "() " ++ block(~children, ());

let testCase = (~name, ~children, ()) =>
  "TEST_CASE(" ++ enquote(name) ++ ") " ++ block(~children, ());

let requireEqual = (~actual, ~expect, ~children, ()) => {j|REQUIRE($actual == $expect);|j};

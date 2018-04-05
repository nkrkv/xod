open Belt;

type code = string;

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
let source = children => join(children, "\n");

let indented = children => children |. joinLines |. indent;

let block = children => ["{", indented(children), "}"] |. joinLines;

let catch2TestCase = (name, children) =>
  "TEST_CASE(" ++ enquote(name) ++ ") " ++ block(children);

let requireEqual = (actual, expected) => {j|REQUIRE($actual == $expected);|j};

open Belt;

let join = (strings, delimiter) => {
  let reduce = (strings: list(string), delimiter: string) =>
    List.reduce(strings, ("", ""), ((acc, delimiter'), s) =>
      (acc ++ delimiter' ++ s, delimiter)
    );
  let (str, _) = reduce(strings, delimiter);
  str;
};

let joinLines = strings => join(strings, "\n");

let indent = (str, n) =>
  Js.String.replaceByRe([%bs.re "/^/gm"], Js.String.repeat(n, " "), str);

let enquote = x => {j|"$x"|j};

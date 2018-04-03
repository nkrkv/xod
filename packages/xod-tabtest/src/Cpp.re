open Belt;

let _join = (strings: list(string), delimiter: string) =>
  List.reduce(strings, ("", ""), ((acc, delimiter'), s) =>
    (acc ++ delimiter' ++ s, delimiter)
  );

let join = (strings, delimiter) => {
  let (str, _) = _join(strings, delimiter);
  str;
};

type ast = [
  | `source(list(ast))
  | `blankLine
  | `literal(string)
  | `literalLine(string)
  | `include_(ast)
  | `quotString(string)
  | `varDecl(ast, string, option(ast))
  | `funcDef(ast, string, list(ast))
  | `macroCallBlock(string, list(ast), list(ast))
];

let source = (x: list(ast)) : ast => `source(x);

let blankLine = `blankLine;

let literal = x => `literal(x);

let literalLine = x => `literalLine(x);

let quotString = x => `quotString(x);

let include_ = (x: ast) : ast => `include_(x);

let includeQuot = x => include_(quotString(x));

let varDecl = (~type_, ~name, ~init=?, ()) => `varDecl((type_, name, init));

let funcDef =
    (~retType=`literal("void"), ~name: string, body: list(ast))
    : ast =>
  `funcDef((retType, name, body));

let macroCallBlock = (name: string, args: list(ast), body: list(ast)) =>
  `macroCallBlock((name, args, body));

let dump = (astNode: ast) : string => {
  let rec dumpList = (xs: list(ast), delimiter: string) =>
    xs |. List.map(dumpNode) |. join(delimiter)
  and dumpInitializer = (initOpt: option(ast)) =>
    switch (initOpt) {
    | None => ""
    | Some(init) => " = " ++ dumpNode(init)
    }
  and dumpNode = node =>
    switch (node) {
    | `source(x) => dumpList(x, "")
    | `blankLine => "\n"
    | `literal(x) => x
    | `literalLine(x) => x ++ "\n"
    | `quotString(x) => "\"" ++ x ++ "\""
    | `include_(x) => "#include " ++ dumpNode(x) ++ "\n"
    | `varDecl(type_, name, initOpt) =>
      dumpNode(type_) ++ " " ++ name ++ dumpInitializer(initOpt) ++ ";\n"
    | `funcDef(retType, name, body) =>
      dumpNode(retType)
      ++ " "
      ++ name
      ++ "() {\n"
      ++ dumpList(body, "")
      ++ "}\n\n"
    | `macroCallBlock(name, args, body) =>
      name
      ++ "("
      ++ dumpList(args, ", ")
      ++ ") {\n"
      ++ dumpList(body, "")
      ++ "}\n\n"
    };
  dumpNode(astNode);
};

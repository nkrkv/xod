open Rebase;

module Endis = {
  type t =
    | Enable
    | Disable
    | Auto;

  let toBoolean = (default, hint) =>
    switch hint {
    | Enable => true
    | Disable => false
    | Auto => default
    };

  let fromString = (tok) =>
    switch tok {
    | "enable" => Enable
    | "disable" => Disable
    | _ => Auto
    };
};

module Pragma = {
  type t = list(string);

  let filterPragmasByFeature = (feature, pragmas: list(t)) =>
    pragmas
    |> List.filter(
         (pragma) =>
           switch pragma {
           | [] => false
           | [feat, ..._] => feat == feature
           }
       );
};


module Code = {
  /*
     ((?!\/\/).)*? matches lines without // before / *
     [\s\S]*? matches anything across lines
     See: https://stackoverflow.com/questions/1068280/javascript-regex-multiline-flag-doesnt-work
     Question is to reduce regex greedieness
   */
  let blockCommentRegexp = {|^(((?!\/\/).)*?)\s*/\*[\s\S]*?\*/|};

  let stripBlockComments = (code) =>
    code |> Revamp.replaceByString(blockCommentRegexp, "$1", ~flags=[MultiLine]);

  let lineCommentRegexp = {|\s*\/\/.*$|};

  let stripLineComments = (code) =>
    code |> Revamp.replaceByString(lineCommentRegexp, "", ~flags=[MultiLine]);

  let stripCppComments = (code) =>
    code |> stripBlockComments |> stripLineComments;

  let pragmaHeadRegexp = {|#\s*pragma\s+XOD\s+|};

  let pragmaLineRegexp = pragmaHeadRegexp ++ ".*";

  let identifierOrStringRegexp = {foo|[\w._-]+|".*?"|foo};

  let enclosingQuotesRegexp = {|^"|"$|};

  let doesReferSymbol = (symbol, code) =>
    code
    |> stripCppComments
    |> Revamp.test("\\b" ++ symbol ++ "\\b", ~flags=[MultiLine]);

  let doesReferTemplateSymbol = (symbol, templateArg, code) =>
    code
    |> stripCppComments
    |> Revamp.test(
         "\\b" ++ symbol ++ "\\s*\\<\\s*" ++ templateArg ++ "\\s*\\>",
         ~flags=[MultiLine]
       );

  let isOutput = (identifier) => Revamp.test({|^output_|}, identifier);

  let tokenizePragma = (pragmaLine: string) : Pragma.t =>
    pragmaLine
    |> Revamp.replaceByString(pragmaHeadRegexp, "")
    |> Revamp.matches(identifierOrStringRegexp)
    |> Seq.map(Revamp.replaceByString(enclosingQuotesRegexp, ""))
    |> List.fromSeq;

  let findXodPragmas = (code) : list(Pragma.t) =>
    code
    |> stripCppComments
    |> Revamp.matches(pragmaLineRegexp)
    |> Seq.map(tokenizePragma)
    |> List.fromSeq;

  /*
      Returns whether a particular #pragma feature enabled, disabled, or set to auto.
      Default is auto
   */
  let lastPragmaEndis = (feature, code) : Endis.t =>
    code
    |> findXodPragmas
    |> Pragma.filterPragmasByFeature(feature)
    |> List.reverse
    |> List.head
    |> (lastPragma) => (
      switch (lastPragma) {
      | Some([_, x, ..._]) => Endis.fromString(x)
      | _ => Endis.Auto
      }
    );
};

let areTimeoutsEnabled = (code) =>
  code
  |> Code.lastPragmaEndis("timeouts")
  |> Endis.toBoolean(Code.doesReferSymbol("setTimeout", code));

let isNodeIdEnabled = (code) =>
  code
  |> Code.lastPragmaEndis("nodeid")
  |> Endis.toBoolean(Code.doesReferSymbol("getNodeId", code));

let isDirtienessEnabled = (code, identifier) =>
  code
  |> Code.findXodPragmas
  |> List.reduce(
       (acc, pragma) =>
         switch pragma {
         | ["dirtieness", hintTok] =>
           Endis.toBoolean(acc, Endis.fromString(hintTok))
         | ["dirtieness", hintTok, ident] when ident == identifier =>
           Endis.toBoolean(acc, Endis.fromString(hintTok))
         | _ => acc
         },
       Code.isOutput(identifier) /* dirtieness enabled on outputs by default */
       || Code.doesReferTemplateSymbol("isInputDirty", identifier, code)
     );

/* For unit tests */
let findXodPragmas = Code.findXodPragmas;
let stripCppComments = Code.stripCppComments;

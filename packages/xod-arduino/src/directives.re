open Rebase;

type hint =
  | Enable
  | Disable
  | Auto;

type pragma = list(string);

/*
    ((?!\/\/).)*? matches lines without // before / *
    [\s\S]*? matches anything across lines
    See: https://stackoverflow.com/questions/1068280/javascript-regex-multiline-flag-doesnt-work
    Question is to reduce regex greedieness
 */
let blockCommentRegexp = {|^(((?!\/\/).)*?)\s*/\*[\s\S]*?\*/|};

let stripBlockComments = (code) =>
  code
  |> Revamp.replaceByString(blockCommentRegexp, "$1", ~flags=[MultiLine]);

let lineCommentRegexp = {|\s*\/\/.*$|};

let stripLineComments = (code) =>
  code |> Revamp.replaceByString(lineCommentRegexp, "", ~flags=[MultiLine]);

let stripCppComments = (code) =>
  code |> stripBlockComments |> stripLineComments;

let pragmaHeadRegexp = {|#\s*pragma\s+XOD\s+|};

let pragmaLineRegexp = pragmaHeadRegexp ++ ".*";

let identifierOrStringRegexp = {foo|[\w._-]+|".*?"|foo};

let enclosingQuotesRegexp = {|^"|"$|};

let tokenizePragma = (pragmaLine: string) : pragma =>
  pragmaLine
  |> Revamp.replaceByString(pragmaHeadRegexp, "")
  |> Revamp.matches(identifierOrStringRegexp)
  |> Seq.map(Revamp.replaceByString(enclosingQuotesRegexp, ""))
  |> List.fromSeq;

let findXodPragmas = (code) : list(pragma) =>
  code
  |> stripCppComments
  |> Revamp.matches(pragmaLineRegexp)
  |> Seq.map(tokenizePragma)
  |> List.fromSeq;

/* Code */
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

/* Pragmas */
let filterPragmasByFeature = (feature, pragmas: list(pragma)) =>
  pragmas
  |> List.filter(
       (pragma) =>
         switch pragma {
         | [] => false
         | [feat, ..._] => feat == feature
         }
     );

let tokenToEndis = (tok) =>
  switch tok {
  | "enable" => Enable
  | "disable" => Disable
  | _ => Auto
  };

let hintToken = (pragma) =>
  switch pragma {
  | [_, hintTok, ...__] => tokenToEndis(hintTok)
  | _ => Auto
  };

/*
    Returns whether a particular #pragma feature enabled, disabled, or set to auto.
    Default is auto
 */
let pragmaHint = (feature, code) : hint =>
  code
  |> findXodPragmas
  |> filterPragmasByFeature(feature)
  |> List.reverse
  |> List.head
  |> (
    (maybePragma) =>
      (
        switch maybePragma {
        | None => []
        | Some(pragma) => pragma
        }
      )
      |> hintToken
  );

let endisToBoolean = (default, hint) =>
  switch hint {
  | Enable => true
  | Disable => false
  | Auto => default
  };

let areTimeoutsEnabled = (code) =>
  code
  |> pragmaHint("timeouts")
  |> endisToBoolean(doesReferSymbol("setTimeout", code));

let isNodeIdEnabled = (code) =>
  code
  |> pragmaHint("nodeid")
  |> endisToBoolean(doesReferSymbol("getNodeId", code));

let isDirtienessEnabled = (code, identifier) =>
  code
  |> findXodPragmas
  |> List.reduce(
       (acc, pragma) =>
         switch pragma {
         | ["dirtieness", hintTok] =>
           endisToBoolean(acc, tokenToEndis(hintTok))
         | ["dirtieness", hintTok, ident] when ident == identifier =>
           endisToBoolean(acc, tokenToEndis(hintTok))
         | _ => acc
         },
       isOutput(identifier) /* dirtieness enabled on outputs by default */
       || doesReferTemplateSymbol("isInputDirty", identifier, code)
     );

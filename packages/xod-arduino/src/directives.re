
open Rebase;
open Revamp;

/*
   TODO: array → list
*/

/*
   ((?!\/\/).)*? matches lines without // before / *
   [\s\S]*? matches anything across lines
   See: https://stackoverflow.com/questions/1068280/javascript-regex-multiline-flag-doesnt-work
   Question is to reduce regex greedieness
*/
let blockCommentRegexp = {|^(((?!\/\/).)*?)\s*/\*[\s\S]*?\*/|};
let stripBlockComments = (code) =>
    code |>
    replaceByString(blockCommentRegexp, "$1", ~flags=[MultiLine]);

let lineCommentRegexp = {|\s*\/\/.*$|};
let stripLineComments = (code) =>
    code |> replaceByString(lineCommentRegexp, "", ~flags=[MultiLine]);

let stripCppComments = (code) =>
    code |> stripBlockComments |> stripLineComments;

let pragmaHeadRegexp = {|#\s*pragma\s+XOD\s+|};
let pragmaLineRegexp = pragmaHeadRegexp ++ ".*";
let identifierOrStringRegexp = {foo|[\w._-]+|".*?"|foo};
let enclosingQuotesRegexp = {|^"|"$|};
let tokenizePragma = (pragmaLine: string): array(string) =>
    pragmaLine |>
    replaceByString(pragmaHeadRegexp, "") |>
    matches(identifierOrStringRegexp) |>
    Seq.map(replaceByString(enclosingQuotesRegexp, "")) |>
    Array.fromSeq;

let findXodPragmas = (code): array(array(string)) =>
    code |>
    stripCppComments |>
    matches(pragmaLineRegexp) |>
    Seq.map(tokenizePragma) |>
    Array.fromSeq;

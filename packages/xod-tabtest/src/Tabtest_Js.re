open Belt;

let generateSuite = (project, patchPath) =>
  Tabtest.generateSuite(project, patchPath)
  |. Resulty.map(files => files |. Map.String.toList |. Js.Dict.fromList)
  |. Either.fromResult;

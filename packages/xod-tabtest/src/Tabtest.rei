open Belt;

type t = {
  /* Filename -> Content map of all C++ files comprising the test suite ready
       for compilation
     */
  sourceFiles: Map.String.t(string),
  /* Shell command argv to compile and run the test suite */
  runCommand: list(string),
};

/* Returns a test suite for the given patch of a project */
let generateSuite: (Project.t, Patch.path) => Resulty.t(t, Js.Exn.t);

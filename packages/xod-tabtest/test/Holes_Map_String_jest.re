open Belt;

open Jest;

open Expect;

describe("Key mapping", () =>
  test("changes keys, preserves data", () => {
    let inMap =
      Map.String.empty
      |. Map.String.set("a", 1)
      |. Map.String.set("b", 2)
      |. Map.String.set("c", 3);
    let outMap = inMap |. Holes.Map.String.mapKeys(Js.String.toUpperCase);
    let expectedMap =
      Map.String.empty
      |. Map.String.set("A", 1)
      |. Map.String.set("B", 2)
      |. Map.String.set("C", 3);
    /* TODO: custom matcher */
    /* TODO: refmt */
    expect(Map.String.eq(outMap, expectedMap, (==))) |> toBe(true);
  })
);

describe("Inner join", () =>
  test("applies transitive associations", () => {
    let left =
      Map.String.empty
      |. Map.String.set("a", "hello")
      |. Map.String.set("b", "thanks")
      |. Map.String.set("c", "bye");
    let right =
      Map.String.empty
      |. Map.String.set("hello", "e")
      |. Map.String.set("thanks", "f")
      |. Map.String.set("bye", "g");
    let outMap = Holes.Map.String.innerJoin(left, right);
    /* TODO: better example */
    let expectedMap =
      Map.String.empty
      |. Map.String.set("a", "e")
      |. Map.String.set("b", "f")
      |. Map.String.set("c", "g");
    /* TODO: refmt */
    /* TODO: custom matcher */
    expect(Map.String.eq(outMap, expectedMap, (==))) |> toBe(true);
  })
);

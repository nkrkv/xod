open Belt;

type t('v) = Map.String.t('v);

let mapKeys = (xs: t('v), fn: string => string) : t('v) =>
  Map.String.toArray(xs)
  |. Array.map(((k, v)) => (fn(k), v))
  |. Map.String.fromArray;

let keepMapWithKey = (xs: t('v), fn: (string, 'v) => option('v2)) : t('v2) =>
  Map.String.toArray(xs)
  |. Array.keepMap(((k, v)) => fn(k, v) |. Option.map(v2 => (k, v2)))
  |. Map.String.fromArray;

let keepMap = (xs, fn) => keepMapWithKey(xs, (_, v) => fn(v));

let innerJoin = (xs: t(string), ys: t(string)) : t(string) =>
  keepMap(xs, v => ys |. Map.String.get(v));

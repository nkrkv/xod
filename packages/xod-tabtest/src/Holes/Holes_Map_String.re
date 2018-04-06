open Belt;

type k = Map.String.key;

type t('v) = Map.String.t('v);

let mapKeys = (xs, fn) =>
  Map.String.toArray(xs)
  |. Array.map(((k, v)) => (fn(k), v))
  |. Map.String.fromArray;

let keepMapWithKey = (xs: t('v), fn: (k, 'v) => option('v2)) : t('v2) =>
  Map.String.toArray(xs)
  |. Array.keepMap(((k, v)) => fn(k, v) |. Option.map(v2 => (k, v2)))
  |. Map.String.fromArray;

let keepMap = (xs: t('v), fn: 'v => option('v2)) : t('v2) =>
  keepMapWithKey(xs, (_, v) => fn(v));

let innerJoin = (xs, ys) => keepMap(xs, v => ys |. Map.String.get(v));

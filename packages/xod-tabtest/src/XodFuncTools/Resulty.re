type t('good, 'bad) = Js.Result.t('good, 'bad);

let map = (res: t('goodA, 'bad), fn: 'goodA => 'goodB) : t('goodB, 'bad) =>
  switch (res) {
  | Ok(good) => Ok(fn(good))
  | Error(bad) => Error(bad)
  };

let flatMap =
    (res: t('goodA, 'bad), fn: 'goodA => t('goodB, 'bad))
    : t('goodB, 'bad) =>
  switch (res |. map(fn)) {
  | Ok(Ok(good)) => Ok(good)
  | Ok(Error(bad)) => Error(bad)
  | Error(bad) => Error(bad)
  };

let liftM2 =
    (
      fn: ('goodA, 'goodB) => t('goodC, 'bad),
      a: t('goodA, 'bad),
      b: t('goodB, 'bad),
    )
    : t('goodC, 'bad) =>
  switch (a, b) {
  | (Ok(goodA), Ok(goodB)) => fn(goodA, goodB)
  | (Error(bad), _)
  | (_, Error(bad)) => Error(bad)
  };

let lift2 =
    (fn: ('goodA, 'goodB) => 'goodC, a: t('goodA, 'bad), b: t('goodB, 'bad))
    : t('goodC, 'bad) =>
  liftM2((goodA, goodB) => Ok(fn(goodA, goodB)), a, b);

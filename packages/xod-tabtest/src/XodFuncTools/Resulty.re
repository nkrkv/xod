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

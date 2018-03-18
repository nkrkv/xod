type t('good, 'bad) = Js.Result.t('good, 'bad);

let map = (fn: 'goodA => 'goodB, res: t('goodA, 'bad)) : t('goodB, 'bad) =>
  switch (res) {
  | Ok(good) => Ok(fn(good))
  | Error(bad) => Error(bad)
  };

let chain =
    (fn: 'goodA => t('goodB, 'bad), res: t('goodA, 'bad))
    : t('goodB, 'bad) =>
  switch (res |> map(fn)) {
  | Ok(Ok(good)) => Ok(good)
  | Ok(Error(bad)) => Error(bad)
  | Error(bad) => Error(bad)
  };

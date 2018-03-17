type t('good, 'bad) = Js.Result.t('good, 'bad);

let map = (fn: 'goodA => 'goodB, res: t('goodA, 'bad)) : t('goodB, 'bad) =>
  switch (res) {
  | Ok(good) => Ok(fn(good))
  | Error(bad) => Error(bad)
  };

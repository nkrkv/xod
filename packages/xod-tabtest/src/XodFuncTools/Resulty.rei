type t('good, 'bad) = Js.Result.t('good, 'bad);

let map: (t('goodA, 'bad), 'goodA => 'goodB) => t('goodB, 'bad);

let flatMap:
  (t('goodA, 'bad), 'goodA => t('goodB, 'bad)) => t('goodB, 'bad);

let lift2:
  (('goodA, 'goodB) => 'goodC, t('goodA, 'bad), t('goodB, 'bad)) =>
  t('goodC, 'bad);

let liftM2:
  (('goodA, 'goodB) => t('goodC, 'bad), t('goodA, 'bad), t('goodB, 'bad)) =>
  t('goodC, 'bad);

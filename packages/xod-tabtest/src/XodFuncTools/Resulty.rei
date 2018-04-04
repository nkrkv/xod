type t('good, 'bad) = Js.Result.t('good, 'bad);

let map: (t('goodA, 'bad), 'goodA => 'goodB) => t('goodB, 'bad);

let flatMap:
  (t('goodA, 'bad), 'goodA => t('goodB, 'bad)) => t('goodB, 'bad);

let lift2:
  (('goodA, 'goodB) => 'goodR, t('goodA, 'bad), t('goodB, 'bad)) =>
  t('goodR, 'bad);

let lift3:
  (
    ('goodA, 'goodB, 'goodC) => 'goodR,
    t('goodA, 'bad),
    t('goodB, 'bad),
    t('goodC, 'bad)
  ) =>
  t('goodR, 'bad);

let liftM2:
  (('goodA, 'goodB) => t('goodR, 'bad), t('goodA, 'bad), t('goodB, 'bad)) =>
  t('goodR, 'bad);

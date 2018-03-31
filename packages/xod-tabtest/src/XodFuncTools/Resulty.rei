type t('good, 'bad) = Js.Result.t('good, 'bad);

let map: (t('goodA, 'bad), 'goodA => 'goodB) => t('goodB, 'bad);

let flatMap:
  (t('goodA, 'bad), 'goodA => t('goodB, 'bad)) => t('goodB, 'bad);

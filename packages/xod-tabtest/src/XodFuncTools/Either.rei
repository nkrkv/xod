type t('left, 'right);

let toResult: t('left, 'right) => Js.Result.t('right, 'left);

let fromResult: Js.Result.t('right, 'left) => t('left, 'right);

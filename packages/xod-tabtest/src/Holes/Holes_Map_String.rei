open Belt;

type t('a) = Map.String.t('a);

let mapKeys: (t('a), string => string) => t('a);

let innerJoin: (t(string), t(string)) => t(string);


import * as R from 'ramda';
import { fromList } from '@glennsl/rebase/lib/js/src/Rebase__Array.bs.js';

import * as Re from '../lib/js/src/directives';

export {
  isDirtienessEnabled,
  isNodeIdEnabled,
  areTimeoutsEnabled,
  stripCppComments,
} from '../lib/js/src/directives';

export const findXodPragmas = R.compose(
  R.map(fromList),
  fromList,
  Re.findXodPragmas
);

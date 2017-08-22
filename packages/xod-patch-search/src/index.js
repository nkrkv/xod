import R from 'ramda';
import Fuse from 'fuse.js';
import createIndexData from './mapper';

const options = {
  shouldSort: true,
  tokenize: true,
  matchAllTokens: true,
  includeScore: true,
  threshold: 0.37,
  location: 0,
  distance: 2000,
  maxPatternLength: 32,
  minMatchCharLength: 2,
  keys: [
    {
      name: 'path',
      weight: 0.7,
    },
    {
      name: 'keywords',
      weight: 0.5,
    },
    {
      name: 'lib',
      weight: 1,
    },
    {
      name: 'description',
      weight: 0.3,
    },
    {
      name: 'fullDescription',
      weight: 0.2,
    },
  ],
};

// :: [IndexData] -> SearchIndex
export const createIndex = data => new Fuse(data, options);
// :: [Patch] -> SearchIndex
export const createIndexFromPatches = R.compose(
  createIndex,
  createIndexData
);

export default createIndex;
export { default as createIndexData } from './mapper';

import { assert } from 'chai';

import createIndex from '../src/index';
import indexData from './fixtures/indexData.json';

describe('xod-patch-search/index', () => {
  const idx = createIndex(indexData);

  it('searches by path correctly', () => {
    const result = idx.search('path:number');
    assert.equal(
      result[0].item,
      'xod/core/constant-number'
    );
  });
  it('searches by lib correctly', () => {
    const result = idx.search('lib:xod/core');
    assert.lengthOf(result, 6);
  });
  it('searches all fields correctly', () => {
    assert.equal(
      idx.search('number')[0].item,
      'xod/core/constant-number' // Cause it has a `number` in the path and it has a higher priority
    );
    assert.equal(
      idx.search('sum')[0].item,
      'xod/core/add' // Cause only this node has a `sum` word in the description
    );

    const meterResult = idx.search('meter');
    assert.equal(
      meterResult[0].item,
      'xod/common-hardware/gp2y0a41-range-meter' // Cause this node has a full match `meter` in the `path`
    );
    assert.equal(
      meterResult[1].item,
      'xod/utils/m-to-cm' // Cause this node has a full match `meter` in the description
    );
  });
});

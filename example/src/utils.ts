export const superclusterOptions = {
  minZoom: 0,
  maxZoom: 16,
  radius: 40,
  extent: 512,
  minPoints: 2,
};

export const PerformanceNow =
  // @ts-ignore
  (global.performance && global.performance.now) ||
  // @ts-ignore
  global.performanceNow ||
  // @ts-ignore
  global.nativePerformanceNow ||
  (() => {
    try {
      var now = require('fbjs/lib/performanceNow');
    } finally {
      return now;
    }
  })();

const fixed = (n: number) => (Math.trunc(n) === n ? n + '' : n.toFixed(3));

export const timeDelta = (start: number, end: number) => fixed(end - start);

export const deepEqualWithoutIds = (x: any, y: any) => {
  if (x === y) return true;
  // if both x and y are null or undefined and exactly the same

  if (!(x instanceof Object) || !(y instanceof Object)) return false;
  // if they are not strictly equal, they both need to be Objects

  if (x.constructor !== y.constructor) return false;
  // they must have the exact same prototype chain, the closest we can do is
  // test there constructor.

  for (var p in x) {
    if (p === 'id' || p === 'cluster_id' || p === 'getExpansionRegion')
      continue;
    // skip id and cluster_id and getExpansionRegion

    if (!x.hasOwnProperty(p)) continue;
    // other properties were tested using x.constructor === y.constructor

    if (!y.hasOwnProperty(p)) return false;

    // point_count_abbreviated can be a number or string in the JS implementation - "2k" vs 20
    if (p === 'point_count_abbreviated' && String(x[p]) === String(y[p]))
      continue;

    // check if the coordinates are close to each other
    if (p === 'coordinates' && Array.isArray(x[p]) && Array.isArray(y[p])) {
      if (x[p].length !== y[p].length) return false;

      for (let i = 0; i < x[p].length; i++) {
        const diff = x[p][i] / y[p][i];
        if (diff > 1.01 || diff < 0.99) {
          return false;
        }
      }
      continue;
    }

    if (x[p] === y[p]) continue;
    // if they have the same strict value or identity then they are equal

    if (typeof x[p] !== 'object') return false;
    // Numbers, Strings, Functions, Booleans must be strictly equal

    if (!deepEqualWithoutIds(x[p], y[p])) return false;
    // Objects and Arrays must be tested recursively
  }

  for (p in y) {
    if (p === 'id' || p === 'cluster_id' || p === 'getExpansionRegion')
      continue;
    // skip id and cluster_id

    if (y.hasOwnProperty(p) && !x.hasOwnProperty(p)) return false;
    // allows x[ p ] to be set to undefined
  }

  return true;
};

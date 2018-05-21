const Promise = require('bluebird');
const _ = require('lodash');
const geolib = require('geolib');
const uuid = require('uuid/v4');
const rp = require('request-promise');

const LOCATION_SHANGHAI = {
  latitude: 31.2304,
  longitude: 121.4737,
};
const POINT_COUNT = 1000;
const SCALE = 2 * 1000;
const ADDRESS = 'http://localhost:8000/GeoIndex/';

function generateLocation(center, scale) {
  const id = uuid();
  let loc = center;

  loc = geolib.computeDestinationPoint(loc, (Math.random() - 0.5) * scale, 0);
  loc = geolib.computeDestinationPoint(loc, (Math.random() - 0.5) * scale, 90);

  const { latitude, longitude } = loc;

  return [id, latitude, longitude];
}

function queryClosestPoints({ id, target, count, radius }) {
  const uri = `${ADDRESS}${id}`;

  return Promise.resolve(rp.post(uri, {
    json: {
      radius,
      count,
      points: [
        target,
      ]
    }
  })).tap(console.log);
}

function createIndex({ center, scale, count }) {
  const points = _.times(count, () => generateLocation(LOCATION_SHANGHAI, scale));

  return Promise.resolve(rp.post(ADDRESS, { json: { points } })).tap(console.log);
}

/*
function performanceTest({
  center,
  totalCount,
  scale,
  queryCounts,
}) {
  createIndex({ center, scale, count: totalCount })
    .then(({
      id,
      '@performance-logs': {
        'build-index': duration,
      },
    }) => {
      _.forEach(queryCounts)
    });
}
*/
createIndex({ center: LOCATION_SHANGHAI, scale: SCALE, count: POINT_COUNT })
  .tap(console.log)
  .then(({ id }) => queryClosestPoints({
    id,
    target: generateLocation(LOCATION_SHANGHAI, SCALE),
    count: 100,
    radius: SCALE * 0.707,
  }))
  .then(({ points, id }) => {
    return rp.delete(`${ADDRESS}${id}`);
  })
  .tap(console.log);


const _ = require('lodash');
const geolib = require('geolib');
const uuid = require('uuid/v4');
const rp = require('request-promise');
const Promise = require('bluebird');

const LOCATION_SHANGHAI = {
  latitude: 31.2304,
  longitude: 121.4737,
};
const POINT_COUNT = 100000;
const SCALE = 2 * 1000;

function generateLocation(center, scale) {
  const id = uuid();
  let loc = center;

  loc = geolib.computeDestinationPoint(loc, (Math.random() - 0.5) * scale, 0);
  loc = geolib.computeDestinationPoint(loc, (Math.random() - 0.5) * scale, 90);

  const { latitude, longitude } = loc;

  return [id, latitude, longitude];
}

const points = _.times(POINT_COUNT, () => generateLocation(LOCATION_SHANGHAI, SCALE));

console.log(`Generated ${points.length} points`);

Promise.resolve(
  rp.post('http://localhost:8000/GeoIndexRedis/', { json: { points } })
)
  .tap(console.log)
  .then(({ id }) => rp.get({
    uri: `http://localhost:8000/GeoIndexRedis/${id}`,
    qs: _.assign({
      radius: SCALE * 1.414,
      count: 100,
    }, _.zipObject(
      ['id', 'latitude', 'longitude'],
      generateLocation(LOCATION_SHANGHAI, SCALE))
    ),
    json: true,
  }))
  .tap(console.log)
  .then(({ id }) => rp.delete(`http://localhost:8000/GeoIndexRedis/${id}`))
  .tap(console.log);

Promise.resolve(
  rp.post('http://localhost:8000/GeoIndex/', { json: { points } })
)
  .tap(console.log)
  .then(({ id }) => rp.get({
    uri: `http://localhost:8000/GeoIndex/${id}`,
    qs: _.assign({
      radius: SCALE * 1.414,
      count: 100,
    }, _.zipObject(
      ['id', 'latitude', 'longitude'],
      generateLocation(LOCATION_SHANGHAI, SCALE))
    ),
    json: true,
  }))
  .tap(console.log)
  .then(({ id }) => rp.delete(`http://localhost:8000/GeoIndex/${id}`))
  .tap(console.log);

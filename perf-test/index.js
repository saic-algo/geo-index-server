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
const ADDRESS = 'http://localhost:8080/GeoIndex/';

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

  return Promise.resolve(rp.get({
    uri,
    qs: {
      radius,
      count,
      latitude: target.latitude,
      longitude: target.longitude,
    },
    json: true,
  })).tap(({ points }) => {
    points.forEach(({ id, latitude, longitude }) => {
      console.log(`id: ${id}, distance: ${geolib.getDistance(target, { latitude, longitude })}`);
    });
  });
}

const points = _.times(POINT_COUNT, () => generateLocation(LOCATION_SHANGHAI, SCALE));

console.log(`Generated ${points.length} points`);

Promise.resolve(
  rp.post(ADDRESS, { json: { points } })
)
  .tap(console.log)
  .then(({ id }) => queryClosestPoints({
    id,
    target: _.zipObject(['id', 'latitude', 'longitude'], generateLocation(LOCATION_SHANGHAI, SCALE)),
    count: 100,
    radius: SCALE * 0.707,
  }))
  .then(({ points, id }) => {
    return rp.delete(`${ADDRESS}${id}`);
  })
  .tap(console.log);


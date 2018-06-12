const Promise = require('bluebird');
const _ = require('lodash');
const geolib = require('geolib');
const uuid = require('uuid/v4');
const rp = require('request-promise');

const LOCATION_SHANGHAI = {
  latitude: 31.2304,
  longitude: 121.4737,
};
const POINT_COUNT = 10000;
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

function queryClosestPoints({ id, targets, count, radius }) {
  const uri = `${ADDRESS}${id}`;

  return Promise.resolve(rp.post(uri, {
    json: {
      radius,
      count,
      points: targets,
    }
  }));
}

function createRandomPoints({ center, scale, count }) {
  return _.times(count, () => generateLocation(center, scale));
}

function createIndex({ points, type }) {
  return Promise.resolve(rp.post(ADDRESS, {
    json: { type, points },
  }));
}

function deleteIndex({ id }) {
  const uri = `${ADDRESS}${id}`;

  return Promise.resolve(rp.delete(uri));
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

function consistencyTest({ points, idRedis, idS2, target, count, radius }) {
  const pointHash = _.zipObject(_.map(points, _.head), _.map(points, _.tail));

  Promise
    .map([idRedis, idS2], id => queryClosestPoints({ id, targets: [target], count, radius }))
    .spread((responseRedis, responseS2) => {
      const [pointsRedis, pointsS2] = _.map([
        responseRedis,
        responseS2,
      ], response => _.map(response.results[0].points, ([id, lat, lng]) => {
        const point = {
          latitude: parseFloat(lat),
          longitude: parseFloat(lng),
        };
        const pointTarget = _.zipObject(['latitude', 'longitude'], _.tail(target));
        const pointInput = _.zipObject(['latitude', 'longitude'], pointHash[id]);
        const distance = geolib.getDistance(point, pointTarget, 0.01, 3);
        const delta = geolib.getDistance(point, pointInput, 0.01, 3);

        return { id, lat, lng, distance, delta };
      }));

      console.log(pointsRedis);
      console.log(pointsS2);
      console.log(_.isEqual(pointsRedis, pointsS2));
    });
}

function performanceTests({ center, scales, totalCounts, queryCounts, queryCount, types }) {
  const results = { Create: [], Query: [] };

  return Promise.mapSeries(scales, scale => Promise.mapSeries(totalCounts, totalCount => {
    console.error(`Generating ${totalCount} points in ${scale}m ^ 2`);

    const points = createRandomPoints({ center, scale, count: totalCount });
    const targets = createRandomPoints({ center, scale, count: queryCount });

    return Promise.mapSeries(types, (type) => {
      const setup = { scale, totalCount, type };

      console.error('Creating index for ', setup);

      return createIndex({ points, type }).tap((res) => {
        results.Create.push(_.defaults({
          duration: res['@performance-logs']['build-index'],
        }, setup));
      }).then(({ id }) => Promise.mapSeries(queryCounts, queryCount => {
        console.error(`Querying closest ${queryCount} points`);

        return queryClosestPoints({
          id,
          targets,
          count: queryCount,
          radius: scale * 0.707,
        }).tap((res) => {
          results.Query.push(_.defaults({
            queryCount,
            duration: res['@performance-logs']['make-query'],
          }, setup));
        });
      }).then(() => deleteIndex({ id })));
    });
  })).then(() => results);
}

performanceTests({
  center: LOCATION_SHANGHAI,
  scales: [5000],
  totalCounts: [100000],
  queryCounts: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100],
  queryCount: 10000,
  types: ['s2'],
}).then(JSON.stringify).then(console.log);

/*
Promise
  .map(['redis', 's2'], type => createIndex({ points, type }))
  .spread(({ id: idRedis }, { id: idS2 }) => consistencyTest({
    points,
    idRedis,
    idS2,
    target: generateLocation(LOCATION_SHANGHAI, SCALE),
    count: 10,
    radius: SCALE * 0.707,
  }));
*/

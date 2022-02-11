# React-native-clusterer

React Native clustering library using a c++ implementation of [supercluster](https://github.com/mapbox/supercluster) and JSI bindings for up to 10x faster initial point loading times than its JavaScript counterpart.

Check out the example for speed comparisons.

## Installation

```sh
npm install react-native-clusterer
```

#### `iOS`

```
cd ios && pod install
```

## Usage

```js
import Supercluster from 'react-native-clusterer';

// ...

const supercluster = new Supercluster(points, options);
const clusters = supercluster.getTile(2, 1, 2)

// ...
// Don't forget to clean up to free memory
// Most likely implementation (on component unmount)
useEffect(() => {
  // ...
  return () => {
    supercluster.destroy();
    // ...
  };
}, []);
```

#### Points

Array of [GeoJSON Feature](https://tools.ietf.org/html/rfc7946#section-3.2) objects. Each feature's `geometry` must be a [GeoJSON Point](https://tools.ietf.org/html/rfc7946#section-3.1.2). Once loaded, index is immutable.

Note: Currently supported Point properties are `null`, `boolean`, `number`, `string`. The rest will be discarded whent the supercluster is created. Other properties can be turned into a JSON and stored as a string.

#### Options

| Option     | Default | Description                                                       |
| ---------- | ------- | ----------------------------------------------------------------- |
| minZoom    | 0       | Minimum zoom level at which clusters are generated.               |
| maxZoom    | 16      | Maximum zoom level at which clusters are generated.               |
| minPoints  | 2       | Minimum number of points to form a cluster.                       |
| radius     | 40      | Cluster radius, in pixels.                                        |
| extent     | 512     | (Tiles) Tile extent. Radius is calculated relative to this value. |
| generateId | false   | Whether to generate ids for input features in vector tiles.       |

## Methods

#### `getClusters(bbox, zoom)`

For the given `bbox` array (`[westLng, southLat, eastLng, northLat]`) and integer `zoom`, returns an array of clusters and points as [GeoJSON Feature](https://tools.ietf.org/html/rfc7946#section-3.2) objects.

#### `getTile(z, x, y)`

For a given zoom and x/y coordinates, returns a [geojson-vt](https://github.com/mapbox/geojson-vt)-compatible JSON tile object with cluster/point features.

#### `getChildren(clusterId)`

Returns the children of a cluster (on the next zoom level) given its id (`cluster_id` value from feature properties).

#### `getLeaves(clusterId, limit = 10, offset = 0)`

Returns all the points of a cluster (given its `cluster_id`), with pagination support:
`limit` is the number of points to return, and `offset` is the number of points to skip (for pagination).

#### `getClusterExpansionZoom(clusterId)`

Returns the zoom on which the cluster expands into several children (useful for "click to zoom" feature) given the cluster's `cluster_id`.

#### `destroy()`

Destroys the c++ cluster and frees its memory

## TO-DOs

- [x] Proper input and return types for methods
- [x] Implement `getClusters(bbox, zoom)`
- [x] Parse and return additional Point properties added by users
- [ ] Find a better implementation for `destroy()`
- [ ] Map/reduce options

## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.

## License

MIT

Uses supercluster for point clustering. Check out [mapbox/supercluster.hpp](https://github.com/mapbox/supercluster.hpp) for additional licensing.

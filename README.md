# React Native Clusterer

The most comprehensive and yet easiest to use react native point clustering library. Uses c++ implementation of [supercluster](https://github.com/mapbox/supercluster) and JSI bindings for up to 10x faster initial point loading times than its JavaScript counterpart.

# Installation

```sh
npm install react-native-clusterer

# iOS
cd ios && pod install
```

# Example

Check out the example folder for a fully functional example and speed comparisons. An android make sure to update AndroidManifest.xml with com.google.android.geo.API_KEY meta data in order for Google Maps to work.

# Usage

You can use this library either as a react native component ([Clusterer](#Clusterer)) for the most hassle free implementation or as a class ([Supercluster](#Supercluster)) for more fine-tuned controls. Supercluster class can be used as a drag-and-drop replacement for JS version with some caveats, check out `destroy()` method and `TO-DOs`.

# Clusterer

```js
//...
import { Clusterer } from 'react-native-clusterer';
import MapView, { Marker } from 'react-native-maps';

// ...
const [markers, setMarkers] = useState([]);
const [region, setRegion] = useState(initialRegion);

// ...
return (
  <MapView
    onRegionChangeComplete={setRegion}
    // ... other props
  >
    <Clusterer
      data={markers}
      region={region}
      options={DEFAULT_OPTIONS}
      mapDimensions={{ width: MAP_WIDTH, height: MAP_HEIGHT }}
      renderItem={(item) => {
        return (
           <Marker
            // ... marker props
          >
            {/*  marker children - callout, custom marker, etc. */}
            {item.properties.cluster ? (
              // render cluster
            ) : (
              // render marker
            )}
          </Marker>
        );
      }}
    />
  </MapView>
);
```

## Props

### `region`

Region from the `<MapView />` Component. Object containing `latitude`, `longitude`, `latitudeDelta` and `longitudeDelta` values.

### `data`

Same as [points](#load(points)) passed to `supercluster.load()`.

### `options`

Same as [options](#Options) for Supercluster.Optional.

### `mapDimensions`

Object containing `width` and `height` of the `<MapView />` Component

### `renderItem`

Function that takes an item (`GeoJSON Feature point or cluster`) and returns a Marker component. `renderItem` additionally provides function getClusterExpansionRegion() inside properies for clusters (ONLY for clusters!)which will return a region that can be used to expand the cluster. Same as [expandCluster](#expandCluster(clusterId)) without the need for `clusterId` param.

# Supercluster

```js
//...
import Supercluster from 'react-native-clusterer';
import MapView, { Marker } from 'react-native-maps';

//...

const [region, setRegion] = useState(initialRegion);
const [clusteredMarkers, setClusteredMarkers] = useState([]);

// Create a new instance of Supercluster
// wrap in useMemo to prevent unnecessary re-renders
const supercluster = new Supercluster(options);

// Load points
useEffect(() => {
  supercluster.load(points);
}, [points]);

// Update clusters on region change
useEffect(() => {
  setClusteredMarkers(
    supercluster.getClustersFromRegion(region, mapDimensions)
  );
}, [region]);

// Don't forget to clean up to free memory
// Most likely implementation (on component unmount)
useEffect(() => {
  // ...
  return () => {
    supercluster.destroy();
    // ...
  };
}, []);

// ...
return (
  <MapView
    onRegionChangeComplete={setRegion}
    // ... other props
  >
  {clusteredMarkers.map(marker => (
          <Marker
            // ... marker props
          >
            {/*
              // ... marker children - callout, custom marker, etc.
            */}
          </Marker>
        );
  )}
    />
  </MapView>
);
```

## Options

| Option     | Default | Description                                                       |
| ---------- | ------- | ----------------------------------------------------------------- |
| minZoom    | 0       | Minimum zoom level at which clusters are generated.               |
| maxZoom    | 16      | Maximum zoom level at which clusters are generated.               |
| minPoints  | 2       | Minimum number of points to form a cluster.                       |
| radius     | 40      | Cluster radius, in pixels.                                        |
| extent     | 512     | (Tiles) Tile extent. Radius is calculated relative to this value. |
| generateId | false   | Whether to generate ids for input features in vector tiles.       |

## Methods

### `load(points)`

Loads an array of [GeoJSON Feature](https://tools.ietf.org/html/rfc7946#section-3.2) objects. Each feature's `geometry` must be a [GeoJSON Point](https://tools.ietf.org/html/rfc7946#section-3.1.2). Once loaded, index is immutable.

Note: Currently supported Point properties are `null`, `boolean`, `number`, `string`. The rest will be discarded whent the supercluster is created. Other properties can be turned into a JSON and stored as a string.

#### `getClusters(bbox, zoom)`

For the given `bbox` array (`[westLng, southLat, eastLng, northLat]`) and integer `zoom`, returns an array of clusters and points as [GeoJSON Feature](https://tools.ietf.org/html/rfc7946#section-3.2) objects.

#### `getClustersFromRegion(region, mapDimensions)`

For the given `region` from react-native-maps `<MapView />` and an object containing `width` and `height` of the component, returns an array of clusters and points as [GeoJSON Feature](https://tools.ietf.org/html/rfc7946#section-3.2) objects.

#### `getTile(z, x, y)`

For a given zoom and x/y coordinates, returns a [geojson-vt](https://github.com/mapbox/geojson-vt)-compatible JSON tile object with cluster/point features.

#### `getChildren(clusterId)`

Returns the children of a cluster (on the next zoom level) given its id (`clusterId` value from feature properties).

#### `getLeaves(clusterId, limit = 10, offset = 0)`

Returns all the points of a cluster (given its `clusterId`), with pagination support:
`limit` is the number of points to return, and `offset` is the number of points to skip (for pagination).

#### `getClusterExpansionZoom(clusterId)`

Returns the zoom on which the cluster expands into several children (useful for "click to zoom" feature) given the cluster's `clusterId`.


#### `expandCluster(clusterId)`
Returns a region containing the center of all the points in a cluster and the delta value by which it should be zoomed out to see all the points. Usefull for animating a MapView after a cluster press.

#### `destroy()`

IMPORTANT: Since JS doesnt have destructors, we have to make sure the cluster stored in c++ memory is also deleted. This method is called automatically when the using the `<Clusterer />` compoenent. Open to any suggestions for Class implementation.

## TO-DOs

- [x] Proper input and return types for methods
- [x] Implement `getClusters(bbox, zoom)`
- [x] Parse and return additional Point properties added by users
- [ ] Find a better implementation for `destroy()`.
- [ ] Map/reduce options

## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.

## License

MIT

Copyright (c) 2021 Jiri Hoffmann

Uses supercluster for point clustering. Check out [mapbox/supercluster.hpp](https://github.com/mapbox/supercluster.hpp) for additional licensing.

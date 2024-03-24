# React Native Clusterer

The most comprehensive and yet easiest to use react native point clustering library. Uses c++ implementation of [supercluster](https://github.com/mapbox/supercluster) and JSI bindings for up to 10x faster initial point loading times than its JavaScript counterpart.

# Installation

```sh
npm install react-native-clusterer

# iOS
cd ios && pod install
```

# Example

Check out the example folder for a fully functional example and speed comparisons. On android make sure to update AndroidManifest.xml with com.google.android.geo.API_KEY meta data in order for Google Maps to work.

# Usage

This library provides three different ways to use Supercluster based on your needs:

- [**useClusterer**](#useclusterer): Hook for most hassle-free implementation.
- [**Clusterer**](#Clusterer): React Native component.
- [**Supercluster**](#Supercluster): Class for custom functionality.

If you are looking for a JS drag-and-drop replacement to speed up point clustering, you should be aware of some caveats:

- Missing `Map/reduce` functionality.

# useClusterer

```js
import { useClusterer } from 'react-native-clusterer';

const MAP_DIMENSIONS =  { width: MAP_WIDTH, height: MAP_HEIGHT }

//...
const [region, setRegion] = useState(initialRegion);
const [points, supercluster] = useClusterer(
  markers,
  MAP_DIMENSIONS,
  region
);

// ...
return (
  <MapView
    onRegionChangeComplete={setRegion}
    // ... other props
  >
  {points.map(point => (
         // These should be memoized components,
         // otherwise you might see flickering
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

## useClusterer Params

### `data`

Same as [points](<#load(points)>) passed to `supercluster.load()`.

### `mapDimensions`

Object containing `width` and `height` of the `<MapView />` Component

### `region`

Region from the `<MapView />` Component: Object containing `latitude`, `longitude`, `latitudeDelta` and `longitudeDelta` values.

### `options`

Same as [options](#Supercluster-options) for Supercluster, not required.

## useClusterer Returns

An array with two elements:

- `points` - Array of points (`GeoJSON Feature point or cluster`). Clusters have an additional getExpansionRegion() which will return a region that can be used to expand the cluster (use [isPointCluster](<#isPointCluster(point)>) to check if this property is defined). Same as [getClusterExpansionRegion](<#getClusterExpansionRegion(clusterId)>) without the need for `clusterId` param.
- `supercluster` - [Supercluster](#Supercluster) instance.

# Clusterer

```js
//...
import { Clusterer } from 'react-native-clusterer';
import MapView, { Marker } from 'react-native-maps';

const MAP_DIMENSIONS =  { width: MAP_WIDTH, height: MAP_HEIGHT }

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
      mapDimensions={MAP_DIMENSIONS}
      renderItem={(item) => {
        return (
         // These should be memoized components,
         // otherwise you might see flickering
           <Marker
            // ... marker props
          >
            {/*  marker children - callout, custom marker, etc. */}
            {item.properties.cluster_id ? (
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

## Clusterer Props

### `data`

Same as [points](<#load(points)>) passed to `supercluster.load()`.

### `mapDimensions`

Object containing `width` and `height` of the `<MapView />` Component

### `region`

Region from the `<MapView />` Component: Object containing `latitude`, `longitude`, `latitudeDelta` and `longitudeDelta` values.

### `options`

Same as [options](#Supercluster-Options) for Supercluster.

### `renderItem`

Function that takes an item (`GeoJSON Feature point or cluster`) and returns a React component.

# Supercluster

```js
import Supercluster from 'react-native-clusterer';
//...

// Create a new instance of Supercluster
const supercluster = new Supercluster(options);

// Load points
supercluster.load(points);

// Get clusters
supercluster.getClustersFromRegion(region, mapDimensions);
```

## Supercluster Options

| Option     | Default | Description                                                       |
| ---------- | ------- | ----------------------------------------------------------------- |
| minZoom    | 0       | Minimum zoom level at which clusters are generated.               |
| maxZoom    | 16      | Maximum zoom level at which clusters are generated.               |
| minPoints  | 2       | Minimum number of points to form a cluster.                       |
| radius     | 40      | Cluster radius, in pixels.                                        |
| extent     | 512     | (Tiles) Tile extent. Radius is calculated relative to this value. |
| generateId | false   | Whether to generate ids for input features in vector tiles.       |

## Supercluster Methods

### `load(points)`

Loads an array of [GeoJSON Feature](https://tools.ietf.org/html/rfc7946#section-3.2) objects. Each feature's `geometry` must be a [GeoJSON Point](https://tools.ietf.org/html/rfc7946#section-3.1.2). Once loaded, index is immutable.

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

#### `getClusterExpansionRegion(clusterId)`

Returns a region containing the center of all the points in a cluster and the delta value by which it should be zoomed out to see all the points. Useful for animating a MapView after a cluster press.

#### `destroy()`

No longer needed (version 1.2.0 and up).

~~Since JS doesnt have destructors, we have to make sure the cluster stored in c++ memory is also deleted. This method is called automatically when using the `<Clusterer />` component.~~

## Utility Methods

#### `isPointCluster(point)`

Typescript type guard for checking if a point is a cluster.

##### **Example**

```js
const _handlePointPress = (point: IFeature) => {
  if (isPointCluster(point)) {
    const toRegion = point.properties.getExpansionRegion();
    mapRef.current?.animateToRegion(toRegion, 500);
  }
};

<Clusterer
  // ... other props
  renderItem={(item) => {
    return <Marker item={item} onPress={handlePointPress} />;
  }}
/>;
```

#### `coordsToGeoJSONFeature(coords, properties)`

Converts coordinates to a GeoJSON Feature object. Accepted formats are `[longitude, latitude]` or `{longitude, latitude}` or `{lng, lat}`. Properties can be anything and are optional.

## TO-DOs

- [x] Proper input and return types for methods
- [x] Implement `getClusters(bbox, zoom)`
- [x] Parse and return additional Point properties added by users
- [x] Find a better implementation for `destroy()`.
- [ ] Map/reduce options

## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.

## License

MIT

Copyright (c) 2021 Jiri Hoffmann

Uses supercluster for point clustering. Check out [mapbox/supercluster.hpp](https://github.com/mapbox/supercluster.hpp) for additional licensing.

import type { Feature, Point } from 'geojson';
import type Supercluster from './types';

export const getMarkersCoordinates = (markers: Feature<Point>) => {
  const [longitude, latitude] = markers.geometry.coordinates;
  return { longitude, latitude };
};

/**
 * Determines if a point is a cluster for `.properties` typesafe accessiblity
 * @param point ClusterFeature or PointFeature
 */
export const isPointCluster = <P, C>(
  point: Supercluster.ClusterFeature<C> | Supercluster.PointFeature<P>
): point is Supercluster.ClusterFeature<C> => {
  return 'properties' in point && 'cluster' in (point.properties as any);
};

type CoordOptions =
  | [number, number]
  | { latitude: number; longitude: number }
  | { lat: number; lng: number };

/**
 * Utility function to convert coordinates to a GeoJSON feature
 * @param coords The coordinates to be converted to a GeoJSON feature - can be an array of two coordinates, `{ lat: number; lng: number }`, or `{ latitude: number; longitude: number }`
 * @param props Additional optional properties to be added to the feature
 */
export const coordsToGeoJSONFeature = <T>(
  coords: CoordOptions,
  props?: T
): Supercluster.PointFeature<T | undefined> => {
  let coordinates;
  if (Array.isArray(coords)) {
    coordinates = coords;
  } else if ('latitude' in coords) {
    coordinates = [coords.longitude, coords.latitude];
  } else {
    coordinates = [coords.lng, coords.lat];
  }

  return {
    type: 'Feature',
    geometry: {
      coordinates,
      type: 'Point',
    },
    properties: props,
  };
};

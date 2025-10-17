import type { Feature, Point } from 'geojson';
import type { Supercluster } from './types';
import type { BBox, LatLng, Region } from './types';

const calculateDelta = (x: number, y: number): number =>
  x > y ? x - y : y - x;

const calculateAverage = (...args: number[]): number => {
  const argList = [...args];
  if (!argList.length) {
    return 0;
  }
  return argList.reduce((sum, num: number) => sum + num, 0) / argList.length;
};

export const regionToBBox = (region: Region): BBox => {
  const lngD =
    region.longitudeDelta < 0
      ? region.longitudeDelta + 360
      : region.longitudeDelta;

  return [
    region.longitude - lngD, // westLng - min lng
    region.latitude - region.latitudeDelta, // southLat - min lat
    region.longitude + lngD, // eastLng - max lng
    region.latitude + region.latitudeDelta, // northLat - max lat
  ];
};

export const getMarkersRegion = (points: LatLng[]): Region => {
  const coordinates = {
    minX: points[0]!.latitude,
    maxX: points[0]!.latitude,
    maxY: points[0]!.longitude,
    minY: points[0]!.longitude,
  };

  const { maxX, minX, maxY, minY } = points.reduce(
    (acc, point) => ({
      minX: Math.min(acc.minX, point.latitude),
      maxX: Math.max(acc.maxX, point.latitude),
      minY: Math.min(acc.minY, point.longitude),
      maxY: Math.max(acc.maxY, point.longitude),
    }),
    { ...coordinates }
  );

  const deltaX = calculateDelta(maxX, minX);
  const deltaY = calculateDelta(maxY, minY);

  return {
    latitude: calculateAverage(minX, maxX),
    longitude: calculateAverage(minY, maxY),
    latitudeDelta: deltaX * 1.5,
    longitudeDelta: deltaY * 1.5,
  };
};

export const getMarkersCoordinates = (markers: Feature<Point>) => {
  const [longitude, latitude] = markers.geometry.coordinates;
  if (!longitude || !latitude) {
    throw new Error(
      `Invalid geometry coordinates: ${JSON.stringify(markers.geometry.coordinates)}`
    );
  }
  return { longitude, latitude };
};

/**
 * Determines if a feature is a cluster for `.properties` typesafe accessiblity
 * @param point ClusterFeature or PointFeature
 */
export const isClusterFeature = <P, C>(
  point:
    | Supercluster.ClusterFeature<C>
    | Supercluster.PointFeature<P>
    | Supercluster.ClusterFeatureBase<C>
): point is Supercluster.ClusterFeature<C> => {
  return (
    'properties' in point &&
    'cluster' in (point.properties as any) &&
    'cluster_id' in (point.properties as any)
  );
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

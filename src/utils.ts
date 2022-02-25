import type { Feature, Point } from 'geojson';
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
  return { longitude, latitude };
};

import type { BBox, Feature, Point } from 'geojson';
import type { LatLng, MapDimensions, Region } from './types';

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

export const getBoundsZoomLevel = (bounds: BBox, dims: MapDimensions, maxZoom: number) => {

  function latRad(lat: number) {
    const sin = Math.sin((lat * Math.PI) / 180);
    const radX2 = Math.log((1 + sin) / (1 - sin)) / 2;
    return Math.max(Math.min(radX2, Math.PI), -Math.PI) / 2;
  }

  function zoom(mapPx: number, worldPx: number, fraction: number) {
    return Math.floor(Math.log(mapPx / worldPx / fraction) / Math.LN2);
  }

  const latFraction = (latRad(bounds[3]) - latRad(bounds[1])) / Math.PI;
  const lngDiff = bounds[2] - bounds[0];
  const lngFraction = (lngDiff < 0 ? lngDiff + 360 : lngDiff) / 360;
  const latZoom = zoom(dims.height, dims.height, latFraction);
  const lngZoom = zoom(dims.width, dims.width, lngFraction);

  return Math.min(latZoom, lngZoom, maxZoom);
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
    latitudeDelta: deltaX * 2,
    longitudeDelta: deltaY * 2,
  };
};

export const getMarkersCoordinates = (markers: Feature<Point>) => {
  const [longitude, latitude] = markers.geometry.coordinates;
  return { longitude, latitude };
};

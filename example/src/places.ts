import { places } from './test/fixtures';

export const initialRegion = {
  latitude: 17.150642213990213,
  latitudeDelta: 102.40413819692193,
  longitude: -90.13384625315666,
  longitudeDelta: 72.32146382331848,
};

export const parsedPlacesData = places.features.map((f, i) => ({
  ...f,
  properties: { ...f.properties, id: i },
}));

export const getRandomNum = (min: number, max: number) => {
  return Math.floor(Math.random() * (max - min + 1)) + min;
};

export const getRandomData = (size: number | string) => {
  return Array.from({ length: parseInt(`${size}`) }, (_, i) => {
    return {
      type: 'Feature' as const,
      geometry: {
        type: 'Point' as const,
        coordinates: [getRandomNum(-180, 180), getRandomNum(-90, 90)],
      },
      properties: {
        id: `point-${i}`,
      },
    };
  });
};

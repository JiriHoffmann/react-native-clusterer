import placesJSON from './places.json';
import placesTile from './places-z0-0-0.json';
import placesTileMin5 from './places-z0-0-0-min5.json';

type Places = {
  type: 'FeatureCollection';
  features: supercluster.PointFeature<any>[];
};

const places = placesJSON as Places;

export { places, placesTile, placesTileMin5 };

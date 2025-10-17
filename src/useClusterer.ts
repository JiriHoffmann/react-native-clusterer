import { useEffect, useState } from 'react';
import type { Supercluster } from './types';
import type { MapDimensions, Region } from './types';
import SuperclusterClass from './Supercluster';
import type * as GeoJSON from 'geojson';

export function useClusterer<
  P extends GeoJSON.GeoJsonProperties = Supercluster.AnyProps,
  C extends GeoJSON.GeoJsonProperties = Supercluster.AnyProps,
>(
  data: Array<Supercluster.PointFeature<P>>,
  mapDimensions: MapDimensions,
  region: Region,
  options?: Supercluster.Options<P, C>
): [
  (Supercluster.PointFeature<P> | Supercluster.ClusterFeature<C>)[],
  SuperclusterClass<P, C>,
] {
  const [supercluster, setSupercluster] = useState(
    new SuperclusterClass(options).load(data)
  );

  const [points, setPoints] = useState(
    supercluster.getClustersFromRegion(region, mapDimensions)
  );

  useEffect(() => {
    setSupercluster(new SuperclusterClass(options).load(data));
    // Keep option properties as individual dependencies in case "options" prop is passed as an inline object
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [
    data,
    options?.extent,
    options?.radius,
    options?.minPoints,
    options?.minZoom,
    options?.maxZoom,
  ]);

  useEffect(() => {
    setPoints(supercluster.getClustersFromRegion(region, mapDimensions));
  }, [supercluster, region, mapDimensions]);

  return [points, supercluster];
}

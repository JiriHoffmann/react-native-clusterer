import { useMemo } from 'react';
import type Supercluster from './types';
import type { MapDimensions, Region } from './types';
import SuperclusterClass from './Supercluster';
import type * as GeoJSON from 'geojson';

export function useClusterer<
  P extends GeoJSON.GeoJsonProperties = Supercluster.AnyProps,
  C extends GeoJSON.GeoJsonProperties = Supercluster.AnyProps
>(
  data: Array<Supercluster.PointFeature<P>>,
  mapDimensions: MapDimensions,
  region: Region,
  options?: Supercluster.Options<P, C>
): [
  (Supercluster.PointFeature<P> | Supercluster.ClusterFeature<C>)[],
  SuperclusterClass<P, C>
] {
  const supercluster = useMemo(
    () => new SuperclusterClass(options).load(data),
    [
      data,
      options?.extent,
      options?.maxZoom,
      options?.minZoom,
      options?.minPoints,
      options?.radius,
    ]
  );

  const points = useMemo(
    () => supercluster.getClustersFromRegion(region, mapDimensions),
    [
      supercluster,
      region.latitude,
      region.longitude,
      region.latitudeDelta,
      region.longitudeDelta,
      mapDimensions.width,
      mapDimensions.height,
    ]
  );

  return [points, supercluster];
}

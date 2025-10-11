import { useState, useEffect } from 'react';
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
  SuperclusterClass<P, C> | undefined
] {
  const [superclusterInstance, setSuperclusterInstance] = useState<
    SuperclusterClass<P, C> | undefined
  >(undefined);

  const [points, setPoints] = useState<
    Supercluster.PointOrClusterFeature<P, C>[]
  >([]);

  useEffect(() => {
    if (data.length > 0) {
      const supercluster = new SuperclusterClass(options).load(data);
      setSuperclusterInstance(supercluster);
    }
  }, [
    data,
    data.length,
    options?.extent,
    options?.maxZoom,
    options?.minZoom,
    options?.minPoints,
    options?.radius,
  ]);

  useEffect(() => {
    if (!superclusterInstance) {
      setPoints([]);
    } else {
      const newPoints = superclusterInstance?.getClustersFromRegion(
        region,
        mapDimensions
      );
      setPoints(newPoints as Supercluster.PointOrClusterFeature<P, C>[]);
    }
  }, [
    superclusterInstance,
    region.latitude,
    region.longitude,
    region.latitudeDelta,
    region.longitudeDelta,
    mapDimensions.width,
    mapDimensions.height,
  ]);

  return [points, superclusterInstance];
}

import React, { FunctionComponent, memo, useEffect, useMemo } from 'react';
import SuperclusterClass from 'react-native-clusterer';
import type Supercluster from './types';
import type { MapDimensions, Region } from './types';

interface ClustererProps<P, C> {
  data: Array<Supercluster.PointFeature<P>>;
  options?: Supercluster.Options<P, C>;
  region: Region;
  mapDimensions: MapDimensions;
  renderItem: (
    cluster:
      | Supercluster.PointFeature<P>
      | Supercluster.ClusterFeatureClusterer<C>
  ) => React.ReactElement;
}

export const Clusterer: FunctionComponent<ClustererProps<GeoJSON.GeoJsonProperties, GeoJSON.GeoJsonProperties>> = memo(
  ({ data, options, region, mapDimensions, renderItem }) => {

    const supercluster = useMemo(
      () => new SuperclusterClass(options).load(data),
      [options, data]
    );

    const markers = useMemo(
      () =>
        supercluster.getClustersFromRegion(region, mapDimensions).map((c) => {
          const cid = c?.properties?.cluster_id;
          if (!cid) return c;

          return {
            ...c,
            properties: {
              ...c.properties,
              getClusterExpansionRegion: () => supercluster.expandCluster(cid),
            },
          };
        }),
      [supercluster, region, mapDimensions]
    );

    useEffect(() => {
      return () => {
        supercluster.destroy();
      };
    }, []);

    return <>{markers.map((c) => renderItem(c))}</>;
  },
  (prevProps, nextProps) =>
    //only if number of points has changed
    prevProps.data.length === nextProps.data.length &&
    // options
    prevProps?.options?.extent === nextProps?.options?.extent &&
    prevProps?.options?.maxZoom === nextProps?.options?.maxZoom &&
    prevProps?.options?.minZoom === nextProps?.options?.minZoom &&
    prevProps?.options?.minPoints === nextProps?.options?.minPoints &&
    prevProps?.options?.radius === nextProps?.options?.radius &&
    // region
    prevProps.region.latitude === nextProps.region.latitude &&
    prevProps.region.longitude === nextProps.region.longitude &&
    prevProps.region.latitudeDelta === nextProps.region.latitudeDelta &&
    prevProps.region.longitudeDelta === nextProps.region.longitudeDelta &&
    // mapDimensions
    prevProps.mapDimensions.width === nextProps.mapDimensions.width &&
    prevProps.mapDimensions.height === nextProps.mapDimensions.height
);

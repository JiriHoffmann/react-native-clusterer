import React, {
  memo,
  ReactElement,
  useEffect,
  useMemo,
  useState,
} from 'react';
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

// @ts-ignore TODO: typefix
export const Clusterer: <
  P extends GeoJSON.GeoJsonProperties = Supercluster.AnyProps,
  C extends GeoJSON.GeoJsonProperties = Supercluster.AnyProps
>(
  p: ClustererProps<P, C>
) => ReactElement = memo(
  ({ data, options, region, mapDimensions, renderItem }) => {
    const [markers, setMarkers] = useState<
      // @ts-ignore TODO: typefix
      (Supercluster.PointFeature<P> | Supercluster.ClusterFeature<C>)[]
    >([]);

    const supercluster = useMemo(
      () => new SuperclusterClass(options).load(data),
      [options, data]
    );

    useEffect(() => {
      const result = supercluster.getClustersFromRegion(region, mapDimensions);

      setMarkers(
        result.map((c) => {
          const cid = c?.properties?.cluster_id;
          if (!cid) return c;

          const getClusterExpansionRegion = () =>
            supercluster.expandCluster(cid);
          return {
            ...c,
            properties: {
              ...c.properties,
              getClusterExpansionRegion,
            },
          };
        })
      );
    }, [data, region, mapDimensions, supercluster]);

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

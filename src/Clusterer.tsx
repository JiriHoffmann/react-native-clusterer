import React, { ReactElement } from 'react';
import { useClusterer } from './useClusterer';
import type Supercluster from './types';
import type { MapDimensions, Region } from './types';

export interface ClustererProps<
  P extends Supercluster.AnyProps,
  C extends Supercluster.AnyProps
> {
  data: Array<Supercluster.PointFeature<P>>;
  mapDimensions: MapDimensions;
  region: Region;
  renderItem: (
    item: Supercluster.PointOrClusterFeature<P, C>,
    index: number,
    array: Supercluster.PointOrClusterFeature<P, C>[]
  ) => React.ReactElement;
  options?: Supercluster.Options<P, C>;
}

export function Clusterer<
  P extends Supercluster.AnyProps,
  C extends Supercluster.AnyProps
>({
  data,
  options,
  region,
  mapDimensions,
  renderItem,
}: ClustererProps<P, C>): ReactElement {
  const [points] = useClusterer(data, mapDimensions, region, options);

  return <>{points.map(renderItem)}</>;
}

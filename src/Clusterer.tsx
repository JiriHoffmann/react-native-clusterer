import React, { FunctionComponent } from 'react';
import type { ClustererProps } from './types';
import { useClusterer } from './useClusterer';

export const Clusterer: FunctionComponent<
  ClustererProps<GeoJSON.GeoJsonProperties, GeoJSON.GeoJsonProperties>
> = ({ data, options, region, mapDimensions, renderItem }) => {
  const [points] = useClusterer(data, mapDimensions, region, options);

  return <>{points.map((c) => renderItem(c))}</>;
};

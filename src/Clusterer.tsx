import React, { FunctionComponent } from 'react';
import { useClusterer } from './useClusterer';

import type * as GeoJSON from 'geojson';
import type { ClustererProps } from './types';

export const Clusterer: FunctionComponent<
  ClustererProps<GeoJSON.GeoJsonProperties, GeoJSON.GeoJsonProperties>
> = ({ data, options, region, mapDimensions, renderItem }) => {
  const [points] = useClusterer(data, mapDimensions, region, options);

  return <>{points.map((c) => renderItem(c))}</>;
};

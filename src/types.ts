// Modified version of:
// Type definitions for supercluster 5.0
// Project: https://github.com/mapbox/supercluster
// Definitions by: Denis Carriere <https://github.com/DenisCarriere>
//                 Nick Zahn <https://github.com/Manc>
// Definitions: https://github.com/DefinitelyTyped/DefinitelyTyped
// TypeScript Version: 2.3

import type * as GeoJSON from 'geojson';

export type BBox = [number, number, number, number];
export interface Region {
  latitude: number;
  longitude: number;
  latitudeDelta: number;
  longitudeDelta: number;
}

export interface MapDimensions {
  width: number;
  height: number;
}

export interface LatLng {
  latitude: number;
  longitude: number;
}

declare namespace Supercluster {
  interface Options<P, C> {
    /**
     * Minimum zoom level at which clusters are generated.
     *
     * @default 0
     */
    minZoom?: number;
    /**
     * Maximum zoom level at which clusters are generated.
     *
     * @default 16
     */
    maxZoom?: number;
    /**
     * Minimum number of points to form a cluster.
     *
     * @default 2
     */
    minPoints?: number;
    /**
     * Cluster radius, in pixels.
     *
     * @default 40
     */
    radius?: number;
    /**
     * (Tiles) Tile extent. Radius is calculated relative to this value.
     *
     * @default 512
     */
    extent?: number;
    /**
     * Whether to generate ids for input features in vector tiles.
     *
     * @default false
     */
    generateId?: boolean;
    /**
     * Size of the KD-tree leaf node. Affects performance.
     *
     * @default 64
     */
    // nodeSize?: number | undefined;
    /**
     * Whether timing info should be logged.
     *
     * @default false
     */
    // log?: boolean | undefined;
    /**
     * A function that returns cluster properties corresponding to a single point.
     *
     * @example
     * (props) => ({sum: props.myValue})
     */
    // map?: ((props: P) => C) | undefined;
    /**
     * A reduce function that merges properties of two clusters into one.
     *
     * @example
     * (accumulated, props) => { accumulated.sum += props.sum; }
     */
    // reduce?: ((accumulated: C, props: Readonly<C>) => void) | undefined;
  }
  /**
   * Default properties type, allowing any properties.
   * Try to avoid this for better typesafety by using proper types.
   */
  interface AnyProps {
    [name: string]: any;
  }
  /**
   * [GeoJSON Feature](https://tools.ietf.org/html/rfc7946#section-3.2),
   * with the geometry being a
   * [GeoJSON Point](https://tools.ietf.org/html/rfc7946#section-3.1.2).
   */
  type PointFeature<P> = GeoJSON.Feature<GeoJSON.Point, P>;
  interface ClusterProperties {
    /**
     * Always `true` to indicate that the Feature is a Cluster and not
     * an individual point.
     */
    cluster: true;
    /** Cluster ID */
    cluster_id: number;
    /** Number of points in the cluster. */
    point_count: number;
    /**
     * Abbreviated number of points in the cluster as string if the number
     * is 1000 or greater (e.g. `1.3k` if the number is `1298`).
     */
    point_count_abbreviated: string;
  }

  type ClusterFeatureBase<C> = PointFeature<ClusterProperties & C>;

  type ClustererClusterProperties = {
    getExpansionRegion: () => Region;
  };

  type ClusterFeature<C> = PointFeature<
    ClusterProperties & C & ClustererClusterProperties
  >;

  type PointOrClusterFeature<P, C> = PointFeature<P> | ClusterFeature<C>;
  interface TileFeature<C, P> {
    type: 1;
    geometry: Array<[number, number]>;
    tags: (ClusterProperties & C) | P;
  }
  interface Tile<C, P> {
    features: Array<TileFeature<C, P>>;
  }
}

export default Supercluster;


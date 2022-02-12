import { NativeModules, Platform } from 'react-native';
import type Supercluster from './types';

const module = NativeModules.Clusterer;

if (module && typeof module.install === 'function' && !(global as any).clustererModule) {
  module.install();
}

const clusterer = (global as any).clustererModule;

export default class SuperclusterClass<
  P extends GeoJSON.GeoJsonProperties = Supercluster.AnyProps,
  C extends GeoJSON.GeoJsonProperties = Supercluster.AnyProps
> {
  private id: string;

  constructor(
    points: Array<Supercluster.PointFeature<P>>,
    options?: Supercluster.Options<P, C>
  ) {
    if (!clusterer) {
      throw new Error(
        `The package 'react-native-clusterer' doesn't seem to be linked. Make sure: \n\n` +
          Platform.select({
            ios: "- You have run 'pod install'\n",
            default: '',
          }) +
          '- You rebuilt the app after installing the package\n' +
          '- You are not using Expo managed workflow\n'
      );
    }

    // generate random id
    this.id = `${Math.floor(
      Math.random() * Math.floor(Math.random() * Date.now())
    )}`;
    clusterer.init(this.id, points, options);
  }

  /**
   * Returns an array of clusters and points as `GeoJSON.Feature` objects
   * for the given bounding box (`bbox`) and zoom level (`zoom`).
   *
   * @param bbox Bounding box (`[westLng, southLat, eastLng, northLat]`).
   * @param zoom Zoom level.
   */
  getClusters(
    bbox: GeoJSON.BBox,
    zoom: number
  ): Array<Supercluster.ClusterFeature<C> | Supercluster.PointFeature<P>> {
    return clusterer.getClusters(this.id, bbox, zoom);
  }

  /**
   * For a given zoom and x/y coordinates, returns a
   * [geojson-vt](https://github.com/mapbox/geojson-vt)-compatible JSON
   * tile object with cluster any point features.
   */
  getTile(x: number, y: number, zoom: number): Supercluster.Tile<C, P> | null {
    return { features: clusterer.getTile(this.id, x, y, zoom) };
  }

  /**
   * Returns the children of a cluster (on the next zoom level).
   *
   * @param clusterId Cluster ID (`cluster_id` value from feature properties).
   * @throws {Error} If `clusterId` does not exist.
   */
  getChildren(
    clusterId: number
  ): Array<Supercluster.ClusterFeature<C> | Supercluster.PointFeature<P>> {
    return clusterer.getChildren(this.id, clusterId);
  }

  /**
   * Returns all the points of a cluster (with pagination support).
   *
   * @param clusterId Cluster ID (`cluster_id` value from feature properties).
   * @param limit The number of points to return (set to `Infinity` for all points).
   * @param offset The amount of points to skip (for pagination).
   */
  getLeaves(
    clusterId: number,
    limit?: number,
    offset?: number
  ): Array<Supercluster.PointFeature<P>> {
    return clusterer.getLeaves(this.id, clusterId, limit ?? 10, offset ?? 0);
  }

  /**
   * Returns the zoom level on which the cluster expands into several
   * children (useful for "click to zoom" feature).
   *
   * @param clusterId Cluster ID (`cluster_id` value from feature properties).
   */
  getClusterExpansionZoom(clusterId: number): number {
    return clusterer.getClusterExpansionZoom(this.id, clusterId);
  }

  /**
   * Destroy the instance.
   */
  destroy(): void {
    clusterer.destroyCluster(this.id);
  }
}

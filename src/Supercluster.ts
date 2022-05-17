import { NativeModules, Platform } from 'react-native';
import GeoViewport from '@mapbox/geo-viewport';
import { getMarkersCoordinates, getMarkersRegion, regionToBBox } from './utils';

import type * as GeoJSON from 'geojson';
import type { MapDimensions, Region } from './types';
import type Supercluster from './types';

const module = NativeModules.Clusterer;

if (
  module &&
  typeof module.install === 'function' &&
  !(global as any).createSupercluster
) {
  module.install();
}

const createSupercluster = (global as any).createSupercluster;
const defaultOptions = {
  minZoom: 0, // min zoom to generate clusters on
  maxZoom: 16, // max zoom level to cluster the points on
  minPoints: 2, // minimum points to form a cluster
  radius: 40, // cluster radius in pixels
  extent: 512, // tile extent (radius is calculated relative to it)
  log: false, // whether to log timing info
  // whether to generate numeric ids for input features (in vector tiles)
  generateId: false,
};

export default class SuperclusterClass<
  P extends GeoJSON.GeoJsonProperties = Supercluster.AnyProps,
  C extends GeoJSON.GeoJsonProperties = Supercluster.AnyProps
> {
  private cppInstance: any = undefined;
  private options: Required<Supercluster.Options<P, C>>;

  constructor(options?: Supercluster.Options<P, C>) {
    if (!createSupercluster) {
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

    this.options = { ...defaultOptions, ...options };
  }

  /**
   * Loads an array of GeoJSON Feature objects. Each feature's geometry
   * must be a GeoJSON Point. Once loaded, index is immutable.
   *
   * @param points Array of GeoJSON Features, the geometries being GeoJSON Points.
   */
  load(points: Array<Supercluster.PointFeature<P>>): SuperclusterClass<P, C> {
    this.cppInstance = createSupercluster(points, this.options);
    return this;
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
    if (!this.cppInstance) return [];

    return this.cppInstance.getClusters(bbox, zoom);
  }

  /**
   * Returns an array of clusters and points as `GeoJSON.Feature` objects
   * for the given bounding box (`bbox`) and zoom level (`zoom`).
   *
   * @param bbox Bounding box (`[westLng, southLat, eastLng, northLat]`).
   * @param zoom Zoom level.
   */
  getClustersFromRegion(
    region: Region,
    mapDimensions: MapDimensions
  ): Array<Supercluster.ClusterFeature<C> | Supercluster.PointFeature<P>> {
    if (!this.cppInstance) return [];

    const bbox = regionToBBox(region);

    if (region.longitudeDelta >= 40)
      return this.cppInstance.getClusters(bbox, this.options.minZoom);

    const viewport = GeoViewport.viewport(
      bbox,
      [mapDimensions.width, mapDimensions.height],
      this.options.minZoom,
      this.options.maxZoom,
      512
    );

    return this.cppInstance.getClusters(bbox, viewport.zoom);
  }

  /**
   * For a given zoom and x/y coordinates, returns a
   * [geojson-vt](https://github.com/mapbox/geojson-vt)-compatible JSON
   * tile object with cluster any point features.
   */
  getTile(x: number, y: number, zoom: number): Supercluster.Tile<C, P> | null {
    if (!this.cppInstance) return null;

    return { features: this.cppInstance.getTile(x, y, zoom) };
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
    if (!this.cppInstance) return [];

    return this.cppInstance.getChildren(clusterId);
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
    if (!this.cppInstance) return [];

    return this.cppInstance.getLeaves(clusterId, limit ?? 10, offset ?? 0);
  }

  /**
   * Returns the zoom level on which the cluster expands into several
   * children (useful for "click to zoom" feature).
   *
   * @param clusterId Cluster ID (`cluster_id` value from feature properties).
   */
  getClusterExpansionZoom(clusterId: number): number {
    if (!this.cppInstance) return 0;

    return this.cppInstance.getClusterExpansionZoom(clusterId);
  }

  /**
   * Returns a region containing the center of all the points in a cluster
   * and the delta value by which it should be zoomed out to see all the points.
   * (usefull for animating a MapView after a cluster press).
   * @param clusterId Cluster ID (`cluster_id` value from feature properties).
   */
  expandCluster = (clusterId: number): Region => {
    if (!this.cppInstance)
      return { latitude: 0, longitude: 0, latitudeDelta: 0, longitudeDelta: 0 };

    const clusterMarkersCoordinates = this.getMarkersInCluster(clusterId).map(
      getMarkersCoordinates
    );

    return getMarkersRegion(clusterMarkersCoordinates);
  };

  /**
   * Destroy the instance.
   * @deprecated Removed in 2.0.0.
   * @returns True if cluster exists and was destroyed, else false.
   */
  destroy(): boolean {
    console.warn(
      'React-Native-Clusterer: destroy function is no longer needed'
    );
    return false;
  }

  private getMarkersInCluster = (
    clusterId: number
  ): Array<Supercluster.PointFeature<GeoJSON.GeoJsonProperties>> => {
    const clusterChildren = this.getChildren(clusterId);

    if (clusterChildren.length > 1) {

      return clusterChildren;
    }
    return this.getMarkersInCluster(clusterChildren[0]!.id as number);
  };
}

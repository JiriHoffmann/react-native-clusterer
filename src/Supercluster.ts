import { NativeModules, Platform } from 'react-native';
import type { GeoJsonProperties } from 'geojson';
import GeoViewport from '@mapbox/geo-viewport';

import type { MapDimensions, Region } from './types';
import type Supercluster from './types';
import { getMarkersCoordinates, getMarkersRegion, regionToBBox } from './utils';

const module = NativeModules.Clusterer;

if (
  module &&
  typeof module.install === 'function' &&
  !(global as any).clustererModule
) {
  module.install();
}

const clusterer = (global as any).clustererModule;

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
  private id: string;
  private loaded: boolean = false;
  private options: Required<Supercluster.Options<P, C>>;

  constructor(options?: Supercluster.Options<P, C>) {
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
    this.options = { ...defaultOptions, ...options };
  }

  /**
   * Loads an array of GeoJSON Feature objects. Each feature's geometry
   * must be a GeoJSON Point. Once loaded, index is immutable.
   *
   * @param points Array of GeoJSON Features, the geometries being GeoJSON Points.
   */
  load(points: Array<Supercluster.PointFeature<P>>): SuperclusterClass<P, C> {
    this.loaded = true;
    clusterer.load(this.id, points, this.options);
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
    if (!this.loaded) return [];

    return clusterer.getClusters(this.id, bbox, zoom);
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
    if (!this.loaded) return [];

    const bbox = regionToBBox(region);
    const viewport = GeoViewport.viewport(
      bbox,
      [mapDimensions.width, mapDimensions.height],
      this.options.minZoom,
      this.options.maxZoom,
      512
    );

    return clusterer.getClusters(this.id, bbox, viewport.zoom);
  }

  /**
   * For a given zoom and x/y coordinates, returns a
   * [geojson-vt](https://github.com/mapbox/geojson-vt)-compatible JSON
   * tile object with cluster any point features.
   */
  getTile(x: number, y: number, zoom: number): Supercluster.Tile<C, P> | null {
    if (!this.loaded) return null;

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
    if (!this.loaded) return [];

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
    if (!this.loaded) return [];

    return clusterer.getLeaves(this.id, clusterId, limit ?? 10, offset ?? 0);
  }

  /**
   * Returns the zoom level on which the cluster expands into several
   * children (useful for "click to zoom" feature).
   *
   * @param clusterId Cluster ID (`cluster_id` value from feature properties).
   */
  getClusterExpansionZoom(clusterId: number): number {
    if (!this.loaded) return 0;

    return clusterer.getClusterExpansionZoom(this.id, clusterId);
  }

  /**
   * Returns a region containing the center of all the points in a cluster
   * and the delta value by which it should be zoomed out to see all the points.
   * (usefull for animating a MapView after a cluster press).
   * @param clusterId Cluster ID (`cluster_id` value from feature properties).
   */
  expandCluster = (clusterId: number): Region => {
    if (!this.loaded)
      return { latitude: 0, longitude: 0, latitudeDelta: 0, longitudeDelta: 0 };

    const clusterMarkersCoordinates = this.getClusterMarkers(clusterId).map(
      getMarkersCoordinates
    );
    return getMarkersRegion(clusterMarkersCoordinates);
  };

  /**
   * Destroy the instance.
   * @returns True if cluster exists and was destroyed, else false.
   */
  destroy(): boolean {
    return clusterer.destroyCluster(this.id);
  }

  private getClusterMarkers = (
    clusterId: number
  ): Array<Supercluster.PointFeature<GeoJsonProperties>> => {
    const clusterChildren = this.getChildren(clusterId);
    if (clusterChildren.length > 1) {
      return clusterChildren;
    }
    return this.getClusterMarkers(clusterChildren[0]!.id as number);
  };
}

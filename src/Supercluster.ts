// import { NativeModules, Platform } from 'react-native';
import GeoViewport from '@mapbox/geo-viewport';
import {
  getMarkersCoordinates,
  getMarkersRegion,
  isPointCluster,
  regionToBBox,
} from './utils';

import type * as GeoJSON from 'geojson';
import type { MapDimensions, Region } from './types';
import type { Supercluster } from './types';

import { NitroModules } from 'react-native-nitro-modules';
import type { Clusterer } from './Clusterer.nitro';

const defaultOptions = {
  minZoom: 0, // min zoom to generate clusters on
  maxZoom: 16, // max zoom level to cluster the points on
  minPoints: 2, // minimum points to form a cluster
  radius: 40, // cluster radius in pixels
  extent: 512, // tile extent (radius is calculated relative to it)
  log: false, // whether to log timing info
  generateId: false, // whether to generate numeric ids for input features (in vector tiles)
};

export default class SuperclusterClass<
  P extends GeoJSON.GeoJsonProperties = Supercluster.AnyProps,
  C extends GeoJSON.GeoJsonProperties = Supercluster.AnyProps,
> {
  private clusterer: any | null = null;
  private options: Required<Supercluster.Options<P, C>>;

  constructor(options?: Supercluster.Options<P, C>) {
    this.options = { ...defaultOptions, ...options };
  }

  /**
   * Loads an array of GeoJSON Feature objects. Each feature's geometry
   * must be a GeoJSON Point. Once loaded, index is immutable.
   *
   * @param points Array of GeoJSON Features, the geometries being GeoJSON Points.
   */
  load(points: Array<Supercluster.PointFeature<P>>): this {
    if (this.clusterer) {
      throw new Error(
        'React-Native-Clusterer: The .load() method can only be called once.'
      );
    }
    this.clusterer = NitroModules.createHybridObject<Clusterer>('Clusterer');
    this.clusterer.load(points, this.options);
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
  ): Array<supercluster.ClusterFeature<C> | supercluster.PointFeature<P>> {
    this.throwIfNotInitialized();

    return this.clusterer
      .getClusters(bbox, zoom)
      .map(this.addExpansionRegionToCluster);
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
  ): Array<supercluster.ClusterFeature<C> | supercluster.PointFeature<P>> {
    this.throwIfNotInitialized();

    const bbox = regionToBBox(region);

    if (region.longitudeDelta >= 40)
      return this.clusterer
        .getClusters(bbox, this.options.minZoom)
        .map(this.addExpansionRegionToCluster);

    const viewport = GeoViewport.viewport(
      bbox,
      [mapDimensions.width, mapDimensions.height],
      this.options.minZoom,
      this.options.maxZoom + 1,
      this.options.extent
    );

    return this.clusterer
      .getClusters(bbox, viewport.zoom)
      .map(this.addExpansionRegionToCluster);
  }

  /**
   * For a given zoom and x/y coordinates, returns a
   * [geojson-vt](https://github.com/mapbox/geojson-vt)-compatible JSON
   * tile object with cluster any point features.
   */
  getTile(x: number, y: number, zoom: number): supercluster.Tile<C, P> | null {
    this.throwIfNotInitialized();

    return { features: this.clusterer.getTile(x, y, zoom) };
  }

  /**
   * Returns the children of a cluster (on the next zoom level).
   *
   * @param clusterId Cluster ID (`cluster_id` value from feature properties).
   * @throws {Error} If `clusterId` does not exist.
   */
  getChildren(
    clusterId: number
  ): Array<supercluster.ClusterFeature<C> | supercluster.PointFeature<P>> {
    this.throwIfNotInitialized();

    return this.clusterer.getChildren(clusterId);
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
  ): Array<supercluster.PointFeature<P>> {
    this.throwIfNotInitialized();

    return this.clusterer.getLeaves(clusterId, limit ?? 10, offset ?? 0);
  }

  /**
   * Returns the zoom level on which the cluster expands into several
   * children (useful for "click to zoom" feature).
   *
   * @param clusterId Cluster ID (`cluster_id` value from feature properties).
   */
  getClusterExpansionZoom(clusterId: number): number {
    this.throwIfNotInitialized();

    return this.clusterer.getClusterExpansionZoom(clusterId);
  }

  /**
   * Returns a region containing the center of all the points in a cluster
   * and the delta value by which it should be zoomed out to see all the points.
   * (usefull for animating a MapView after a cluster press).
   * @param clusterId Cluster ID (`cluster_id` value from feature properties).
   */
  getClusterExpansionRegion = (clusterId: number): Region => {
    this.throwIfNotInitialized();

    const clusterMarkersCoordinates = this.getMarkersInCluster(clusterId).map(
      getMarkersCoordinates
    );

    return getMarkersRegion(clusterMarkersCoordinates);
  };

  private throwIfNotInitialized(): void {
    if (!this.clusterer) {
      throw new Error(
        'React-Native-Clusterer: this Supercluster has not features. Use the load() method to add features.'
      );
    }
  }

  private getMarkersInCluster = (
    clusterId: number
  ): Array<supercluster.PointFeature<GeoJSON.GeoJsonProperties>> => {
    const clusterChildren = this.getChildren(clusterId);

    if (clusterChildren.length > 1) {
      return clusterChildren;
    }
    return this.getMarkersInCluster(clusterChildren[0]!.id as number);
  };

  private addExpansionRegionToCluster = (
    feature: supercluster.PointFeature<P> | Supercluster.ClusterFeatureBase<C>
  ) => {
    if (isPointCluster(feature)) {
      feature.properties.getExpansionRegion = () =>
        this.getClusterExpansionRegion(feature.properties!.cluster_id);
    }
    return feature;
  };
}

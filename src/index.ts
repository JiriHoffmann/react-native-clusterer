import { NativeModules, Platform } from 'react-native';

const module = NativeModules.Clusterer;

if (module && typeof module.install === 'function') {
  module.install();
}

// @ts-ignore
const clusterer = (global as any).clustererModule;

interface SuperclusterOptions {
  radius?: number;
  maxZoom?: number;
  minZoom?: number;
  extent?: number;
  minSize?: number;
  generateId?: boolean;
}

export default class Supercluster {
  private id: string;

  constructor(points: any, options?: SuperclusterOptions) {
    if (!clusterer) {
      throw new Error(
        `The package 'react-native-cryptopp' doesn't seem to be linked. Make sure: \n\n` +
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

  // TODO: 
  // getClusters(bbox:any, zoom: number): any {
  //   return module.getClusters(this.id, bbox, zoom);
  // }

  getTile(x: number, y: number, z: number): any {
    return clusterer.getTile(this.id, x, y, z);
  }

  getChildren(clusterId: string): any {
    return clusterer.getChildren(this.id, clusterId);
  }

  getLeaves(clusterId: string, limit?: number, offset?: number): any {
    return clusterer.getLeaves(this.id, clusterId, limit ?? 10, offset ?? 0);
  }

  getClusterExpansionZoom(clusterId: string): number {
    return clusterer.getClusterExpansionZoom(this.id, clusterId);
  }

  destroy(): void {
    clusterer.destroyCluster(this.id);
  }
}

import { NativeModules, Platform } from 'react-native';

const module = NativeModules.Clusterer;

if (module && typeof module.install === 'function') {
  module.install();
}

// @ts-ignore
const clusterer = global.clustererModule;

export default class Supercluster {
  id;

  constructor(points, options) {
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

  getClusters(bbox, zoom) {
    return clusterer.getClusters(this.id, bbox, zoom);
  }

  getTile(x, y, z) {
    return { features: clusterer.getTile(this.id, x, y, z) };
  }

  getChildren(clusterId) {
    return clusterer.getChildren(this.id, clusterId);
  }

  getLeaves(clusterId, limit, offset) {
    return clusterer.getLeaves(this.id, clusterId, limit ?? 10, offset ?? 0);
  }

  getClusterExpansionZoom(clusterId) {
    return clusterer.getClusterExpansionZoom(this.id, clusterId);
  }

  destroy() {
    clusterer.destroyCluster(this.id);
  }
}

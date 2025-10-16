import { type FunctionComponent } from 'react';
import { StyleSheet, View, Text } from 'react-native';

import Supercluster from 'react-native-clusterer';
import SuperclusterJS from 'supercluster';
import { places, placesTile, placesTileMin5 } from './test/fixtures';
import { deepEqualWithoutIds, superclusterOptions } from './utils';
import { getRandomData } from './places';

export const Tests: FunctionComponent<{}> = () => {
  const generatesClustersProperly = () => {
    const index = new Supercluster().load(places.features);
    const tile = index.getTile(0, 0, 0);
    return deepEqualWithoutIds(tile, placesTile);
  };

  const supportsMinPointsOption = () => {
    const index = new Supercluster({ minPoints: 5 }).load(places.features);
    const tile = index.getTile(0, 0, 0);
    return deepEqualWithoutIds(tile?.features, placesTileMin5.features);
  };

  const returnChildrenOfACluster = () => {
    const index = new Supercluster().load(places.features);
    const childCounts = index
      .getChildren(1)
      .map((p) => p.properties.point_count || 1);
    return deepEqualWithoutIds(childCounts, [6, 7, 2, 1]);
  };

  const returnLeavesOfACluster = () => {
    const index = new Supercluster().load(places.features);
    const leafNames = index.getLeaves(1, 10, 5).map((p) => p.properties.name);
    return deepEqualWithoutIds(leafNames, [
      'Niagara Falls',
      'Cape San Blas',
      'Cape Sable',
      'Cape Canaveral',
      'San  Salvador',
      'Cabo Gracias a Dios',
      'I. de Cozumel',
      'Grand Cayman',
      'Miquelon',
      'Cape Bauld',
    ]);
  };

  const generatesUniqueIdsWithGenerateIdOption = () => {
    const index = new Supercluster({ generateId: true }).load(places.features);
    const ids = index
      .getTile(0, 0, 0)!
      .features.filter((f) => !f.tags.cluster)
      .map((f) => (f as any).id);
    return deepEqualWithoutIds(
      ids,
      [12, 20, 21, 22, 24, 28, 30, 62, 81, 118, 119, 125, 81, 118]
    );
  };

  const getLeavesHandlesNullPropertyFeatures = () => {
    const index = new Supercluster({ radius: 1 }).load([
      {
        type: 'Feature',
        properties: null as any,
        geometry: {
          type: 'Point',
          coordinates: [-79.04411780507252, 43.08771393436908],
        },
      },
      {
        type: 'Feature',
        properties: null as any,
        geometry: {
          type: 'Point',
          coordinates: [-79.04511780507252, 43.08781393436908],
        },
      },
    ]);
    const leaves = index.getLeaves(1, Infinity, 0);
    return leaves[0]?.properties === null;
  };

  const returnsClusterExpansionZoom = () => {
    const index = new Supercluster().load(places.features);

    return (
      index.getClusterExpansionZoom(2) === 2 &&
      index.getClusterExpansionZoom(2978) === 3 &&
      index.getClusterExpansionZoom(516) === 5 &&
      index.getClusterExpansionZoom(3014) === 6
    );
  };

  const returnsClusterExpansionZoomForMaxZoom = () => {
    const index = new Supercluster({
      radius: 60,
      extent: 256,
      maxZoom: 4,
    }).load(places.features);

    return index.getClusterExpansionZoom(2341) === 5;
  };

  const returnsClustersWhenQueryCrossesInternationalDateline = () => {
    const index = new Supercluster().load([
      {
        type: 'Feature',
        properties: {},
        geometry: {
          type: 'Point',
          coordinates: [-178.989, 0],
        },
      },
      {
        type: 'Feature',
        properties: {},
        geometry: {
          type: 'Point',
          coordinates: [-178.99, 0],
        },
      },
      {
        type: 'Feature',
        properties: {},
        geometry: {
          type: 'Point',
          coordinates: [-178.991, 0],
        },
      },
      {
        type: 'Feature',
        properties: {},
        geometry: {
          type: 'Point',
          coordinates: [-178.992, 0],
        },
      },
    ]);

    const nonCrossing = index.getClusters([-179, -10, -177, 10], 1);
    const crossing = index.getClusters([179, -10, -177, 10], 1);

    return (
      nonCrossing.length > 0 &&
      crossing.length > 0 &&
      nonCrossing.length === crossing.length
    );
  };

  const doesNotCrashOnWeirdBboxValues = () => {
    const index = new Supercluster().load(places.features);
    return (
      index.getClusters([129.42639, -103.720017, -445.930843, 114.518236], 1)
        .length === 26 &&
      index.getClusters([112.207836, -84.578666, -463.149397, 120.169159], 1)
        .length === 27 &&
      index.getClusters([129.886277, -82.33268, -445.470956, 120.39093], 1)
        .length === 26 &&
      index.getClusters([458.220043, -84.239039, -117.13719, 120.206585], 1)
        .length === 25 &&
      index.getClusters([456.713058, -80.354196, -118.644175, 120.539148], 1)
        .length === 25 &&
      index.getClusters([453.105328, -75.857422, -122.251904, 120.73276], 1)
        .length === 25 &&
      index.getClusters([-180, -90, 180, 90], 1).length === 61
    );
  };

  const doesNotCrashOnNonIntegerZoomValues = () => {
    const index = new Supercluster().load(places.features);
    return (
      typeof index.getClusters([179, -10, -177, 10], 1.25).length === 'number'
    );
  };

  const makeSureSameLocationPointsAreClustered = () => {
    const index = new Supercluster({
      maxZoom: 20,
      extent: 8192,
      radius: 16,
    }).load([
      {
        type: 'Feature',
        geometry: { type: 'Point', coordinates: [-1.426798, 53.943034] },
        properties: {},
      },
      {
        type: 'Feature',
        geometry: { type: 'Point', coordinates: [-1.426798, 53.943034] },
        properties: {},
      },
    ]);
    const clusters = index.getClusters([-180, -85, 180, 85], 1);
    return clusters[0]?.properties.point_count === 2;
  };

  const makesSureUnclusteredPointCoordsAreNotRounded = () => {
    const index = new Supercluster({ maxZoom: 19 }).load([
      {
        type: 'Feature',
        geometry: {
          type: 'Point',
          coordinates: [173.19150559062456, -41.340357424709275],
        },
        properties: {},
      },
    ]);

    return deepEqualWithoutIds(
      index.getTile(20, 1028744, 656754)!.features[0]?.geometry[0],
      [421, 281]
    );
  };

  const doesNotThrowOnZeroItems = () => {
    const index = new Supercluster().load([]);
    return deepEqualWithoutIds(index.getClusters([-180, -85, 180, 85], 0), []);
  };

  const resultsAreTheSameAsJS = () => {
    const clusterCPP = new Supercluster(superclusterOptions).load(
      places.features
    );
    const clusterJS = new SuperclusterJS(superclusterOptions).load(
      places.features
    );
    return deepEqualWithoutIds(
      clusterCPP.getClusters([-180, -85, 180, 85], 0),
      clusterJS.getClusters([-180, -85, 180, 85], 0)
    );
  };

  const randomDataResultsAreTheSameAsJS = () => {
    const randomData = getRandomData(100);
    const clusterCPP = new Supercluster(superclusterOptions).load(randomData);
    const clusterJS = new SuperclusterJS(superclusterOptions).load(randomData);
    const dataCPP = clusterCPP.getClusters([-180, -85, 180, 85], 0);
    const dataJS = clusterJS.getClusters([-180, -85, 180, 85], 0);
    return deepEqualWithoutIds(dataCPP, dataJS);
  };

  return (
    <View style={styles.container}>
      <Text>
        generates clusters properly {generatesClustersProperly() ? '✅' : '❌'}
      </Text>
      <Text>
        supports minPoints option {supportsMinPointsOption() ? '✅' : '❌'}
      </Text>
      <Text>
        return children of a cluster {returnChildrenOfACluster() ? '✅' : '❌'}
      </Text>
      <Text>
        return leaves of a cluster {returnLeavesOfACluster() ? '✅' : '❌'}
      </Text>
      <Text>
        generates unique ids with generateId option{' '}
        {generatesUniqueIdsWithGenerateIdOption() ? '✅' : '❌'}
      </Text>
      <Text>
        getLeaves handles null property features{' '}
        {getLeavesHandlesNullPropertyFeatures() ? '✅' : '❌'}
      </Text>
      <Text>
        returns cluster expansion zoom{' '}
        {returnsClusterExpansionZoom() ? '✅' : '❌'}
      </Text>
      <Text>
        returns cluster expansion zoom for maxZoom{' '}
        {returnsClusterExpansionZoomForMaxZoom() ? '✅' : '❌'}
      </Text>
      <Text>
        returns clusters when query crosses international dateline{' '}
        {returnsClustersWhenQueryCrossesInternationalDateline() ? '✅' : '❌'}
      </Text>
      <Text>
        does not crash on weird bbox values{' '}
        {doesNotCrashOnWeirdBboxValues() ? '✅' : '❌'}
      </Text>
      <Text>
        does not crash on non-integer zoom values{' '}
        {doesNotCrashOnNonIntegerZoomValues() ? '✅' : '❌'}
      </Text>
      <Text>
        make sure same location points are clustered{' '}
        {makeSureSameLocationPointsAreClustered() ? '✅' : '❌'}
      </Text>
      <Text>
        makes sure unclustered point coords are not rounded{' '}
        {makesSureUnclusteredPointCoordsAreNotRounded() ? '✅' : '❌'}
      </Text>
      <Text>
        does not throw on zero items {doesNotThrowOnZeroItems() ? '✅' : '❌'}
      </Text>

      <Text>
        results are the same as JS {resultsAreTheSameAsJS() ? '✅' : '❌'}
      </Text>
      <Text>
        random data results are the same as JS{' '}
        {randomDataResultsAreTheSameAsJS() ? '✅' : '❌'}
      </Text>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    marginTop: 10,
    marginHorizontal: 10,
    gap: 20,
  },
  h2: {
    fontSize: 16,
    fontWeight: 'bold',
  },
  buttonContainer: {
    position: 'absolute',
    bottom: 20,
    left: 0,
    right: 0,
    flexDirection: 'row',
    justifyContent: 'space-around',
    width: '100%',
  },
  defaultDataButton: {
    marginVertical: 5,
    borderRadius: 5,
    justifyContent: 'center',
    alignItems: 'center',
    height: 40,
    backgroundColor: '#8eb3ed',
  },
  button: {
    flex: 1,
    borderRadius: 5,
    justifyContent: 'center',
    alignItems: 'center',
    height: 40,
    backgroundColor: '#8eb3ed',
  },
  inputContainer: {
    flexDirection: 'row',
    marginBottom: 10,
  },
  input: {
    flex: 1,
    borderWidth: 1,
    borderRadius: 5,
    borderColor: '#8eb3ed',
    height: 40,
  },
  type: {
    flex: 1,
    borderRadius: 5,
    justifyContent: 'center',
    alignItems: 'center',
    height: 40,
    borderWidth: 2,
    marginTop: 20,
  },
});

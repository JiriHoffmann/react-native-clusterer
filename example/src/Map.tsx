import { useState, useRef } from 'react';
import { Dimensions, StyleSheet, View } from 'react-native';
import {
  Clusterer,
  isPointCluster,
  type Supercluster,
} from 'react-native-clusterer';
import MapView, { type Region } from 'react-native-maps';
import { initialRegion, parsedPlacesData } from './places';
import { Point } from './Point';

type IFeature = Supercluster.PointOrClusterFeature<any, any>;

const MAP_WIDTH = Dimensions.get('window').width;
const MAP_HEIGHT = Dimensions.get('window').height - 80;
const MAP_DIMENSIONS = { width: MAP_WIDTH, height: MAP_HEIGHT };

export const Map = () => {
  const [region, setRegion] = useState<Region>(initialRegion);
  const mapRef = useRef<MapView>(null);

  const _handlePointPress = (point: IFeature) => {
    if (isPointCluster(point)) {
      const toRegion = point.properties.getExpansionRegion();
      mapRef.current?.animateToRegion(toRegion, 500);
    }
  };

  return (
    <View style={styles.container}>
      <MapView
        ref={mapRef}
        initialRegion={initialRegion}
        onRegionChangeComplete={setRegion}
        style={MAP_DIMENSIONS}
      >
        <Clusterer
          data={parsedPlacesData}
          region={region}
          options={{ radius: 18 }}
          mapDimensions={MAP_DIMENSIONS}
          renderItem={(item) => {
            return (
              <Point
                key={
                  isPointCluster(item)
                    ? `cluster-${item.properties.cluster_id}`
                    : `point-${item.properties.id}`
                }
                item={item}
                onPress={_handlePointPress}
              />
            );
          }}
        />
      </MapView>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
});

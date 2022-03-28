import React, { useState, useCallback, useRef } from 'react';
import { Dimensions, StyleSheet, View } from 'react-native';
import { Clusterer } from 'react-native-clusterer';
import type { supercluster } from 'react-native-clusterer';
import MapView, { Region } from 'react-native-maps';
import { initialRegion, parsedPlacesData } from './places';
import { Point } from './Point';

const MAP_WIDTH = Dimensions.get('window').width;
const MAP_HEIGHT = Dimensions.get('window').height - 80;

export const Map = () => {
  const [region, setRegion] = useState<Region>(initialRegion);
  const mapRef = useRef<MapView>(null);

  const _handlePointPress = useCallback(
    (
      point:
        | supercluster.PointFeature<GeoJSON.GeoJsonProperties>
        | supercluster.ClusterFeatureClusterer<GeoJSON.GeoJsonProperties>
    ) => {
      if (point.properties?.getClusterExpansionRegion) {
        const region = point.properties?.getClusterExpansionRegion();
        mapRef.current?.animateToRegion(region, 500);
      }
    },
    [mapRef]
  );

  return (
    <View style={styles.container}>
      <MapView
        ref={mapRef}
        initialRegion={initialRegion}
        onRegionChangeComplete={setRegion}
        style={{
          width: MAP_WIDTH,
          height: MAP_HEIGHT,
        }}
      >
        <Clusterer
          data={parsedPlacesData}
          region={region}
          options={{ radius: 18 }}
          mapDimensions={{ width: MAP_WIDTH, height: MAP_HEIGHT }}
          renderItem={(item) => {
            return (
              <Point
                key={
                  item.properties?.cluster_id ?? `point-${item.properties?.id}`
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

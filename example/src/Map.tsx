import React, { useState, useCallback, useRef } from 'react';
import { Dimensions, Text, StyleSheet, View } from 'react-native';
import { Clusterer } from 'react-native-clusterer';
import type { supercluster } from 'react-native-clusterer';
import MapView, { Region, Marker, Callout } from 'react-native-maps';
import { initialRegion, parsedPlacesData } from './places';

const MAP_WIDTH = Dimensions.get('window').width;
const MAP_HEIGHT = Dimensions.get('window').height - 100;

export const Map = () => {
  const [region, setRegion] = useState<Region>(initialRegion);
  const mapRef = useRef<MapView>(null);

  const _handleClusterPress = useCallback(
    (
      cluster:
        | supercluster.PointFeature<GeoJSON.GeoJsonProperties>
        | supercluster.ClusterFeatureClusterer<GeoJSON.GeoJsonProperties>
    ) => {
      if (cluster.properties?.getClusterExpansionRegion) {
        const region = cluster.properties?.getClusterExpansionRegion();
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
              <Marker
                key={
                  item.properties?.cluster_id ?? `point-${item.properties?.id}`
                }
                coordinate={{
                  latitude: item.geometry.coordinates[1],
                  longitude: item.geometry.coordinates[0],
                }}
                onPress={() => _handleClusterPress(item)}
              >
                {item.properties?.cluster ? (
                  // Render Cluster
                  <View style={styles.clusterMarker}>
                    <Text style={styles.clusterMarkerText}>
                      {item.properties.point_count}
                    </Text>
                  </View>
                ) : (
                  // Else, use default behavior to render
                  // a marker and add a callout to it
                  <Callout>
                    <View style={styles.calloutContainer}>
                      <Text>{JSON.stringify(item.properties)}</Text>
                    </View>
                  </Callout>
                )}
              </Marker>
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
  calloutContainer: {
    width: 200,
    height: 200,
    backgroundColor: '#fff',
    borderRadius: 5,
    padding: 10,
  },
  clusterMarker: {
    width: 40,
    height: 40,
    borderRadius: 20,
    backgroundColor: '#8eb3ed',
    justifyContent: 'center',
    alignItems: 'center',
  },
  clusterMarkerText: {
    color: '#fff',
    fontSize: 16,
  },
});

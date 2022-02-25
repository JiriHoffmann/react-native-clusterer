/* eslint-disable react-native/no-inline-styles */
import React, { useCallback, useEffect, useMemo, useRef } from 'react';
import { useState } from 'react';
import { Dimensions, Text } from 'react-native';
import { StyleSheet, View } from 'react-native';
import MapView, { Region, Marker, Callout } from 'react-native-maps';
import Supercluster from 'react-native-clusterer';
import { initialRegion, parsedPlacesData } from './places';

const MAP_WIDTH = Dimensions.get('window').width;
const MAP_HEIGHT = Dimensions.get('window').height - 100;

export const Map = () => {
  const [region, setRegion] = useState<Region>(initialRegion);
  const [markers, setMarkers] = useState<any>([]);
  const mapRef = useRef<MapView>(null);

  // Use memo to avoid re-rendering the map when the region changes.
  const supercluster = useMemo(
    () =>
      new Supercluster({ radius: 20, maxZoom: 15, minZoom: 1 }).load(
        parsedPlacesData
      ),
    []
  );

  useEffect(() => {
    return () => {
      supercluster.destroy();
    };
  }, []);

  const _handleClusterPress = useCallback(
    (cluster: any) => {
      const region = supercluster.expandCluster(cluster.properties.cluster_id);
      mapRef.current?.animateToRegion(region, 500);
    },
    [supercluster, mapRef]
  );

  const _handleRegionChangeComplete = useCallback(
    (region: Region) => {
      setMarkers(
        supercluster.getClustersFromRegion(region, {
          width: MAP_WIDTH,
          height: MAP_HEIGHT,
        })
      );
      setRegion(region);
    },
    [supercluster, setMarkers, setRegion]
  );

  return (
    <View style={styles.container}>
      <MapView
        ref={mapRef}
        region={region}
        onRegionChangeComplete={_handleRegionChangeComplete}
        style={{
          width: MAP_WIDTH,
          height: MAP_HEIGHT,
        }}
      >
        {markers.map((cluster: any) => {
          return (
            <Marker
              key={cluster.properties.cluster_id ?? cluster.properties.id}
              coordinate={{
                latitude: cluster.geometry.coordinates[1],
                longitude: cluster.geometry.coordinates[0],
              }}
              onPress={() =>
                cluster.properties.cluster_id && _handleClusterPress(cluster)
              }
            >
              {cluster.properties.cluster ? (
                // Render Cluster
                <View style={styles.clusterMarker}>
                  <Text style={styles.clusterMarkerText}>
                    {cluster.properties.point_count}
                  </Text>
                </View>
              ) : (
                // Else, use default behavior to render
                // a marker and add a callout to it
                <Callout>
                  <View style={styles.calloutContainer}>
                    <Text>{JSON.stringify(cluster.properties)}</Text>
                  </View>
                </Callout>
              )}
            </Marker>
          );
        })}
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

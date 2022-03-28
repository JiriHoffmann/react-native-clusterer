import React, { FunctionComponent, memo } from 'react';
import { Text, StyleSheet, View } from 'react-native';

import type { supercluster } from 'react-native-clusterer';
import { Marker as MapsMarker, Callout } from 'react-native-maps';

type IPoint =
  | supercluster.PointFeature<GeoJSON.GeoJsonProperties>
  | supercluster.ClusterFeatureClusterer<GeoJSON.GeoJsonProperties>;

interface Props {
  item: IPoint;
  onPress: (item: IPoint) => void;
}

export const Point: FunctionComponent<Props> = memo(
  ({ item, onPress }) => {
    return (
      <MapsMarker
        key={item.properties?.cluster_id ?? `point-${item.properties?.id}`}
        coordinate={{
          latitude: item.geometry.coordinates[1],
          longitude: item.geometry.coordinates[0],
        }}
        tracksViewChanges={false}
        onPress={() => onPress(item)}
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
      </MapsMarker>
    );
  },
  (prevProps, nextProps) =>
    prevProps.item.properties?.cluster_id ===
      nextProps.item.properties?.cluster_id &&
    prevProps.item.properties?.id === nextProps.item.properties?.id &&
    prevProps.item.properties?.point_count ===
      nextProps.item.properties?.point_count &&
    prevProps.item.properties?.onItemPress ===
      nextProps.item.properties?.onItemPress
);

const styles = StyleSheet.create({
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

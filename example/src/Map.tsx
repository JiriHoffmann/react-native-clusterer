import { useState, useRef } from 'react';
import { Button, Dimensions, StyleSheet, View } from 'react-native';
import {
  Clusterer,
  isClusterFeature,
  type Supercluster,
} from 'react-native-clusterer';
import MapView, { type Region } from 'react-native-maps';
import { getRandomData, initialRegion, parsedPlacesData } from './places';
import { Point } from './Point';

type IFeature = Supercluster.PointOrClusterFeature<any, any>;

const MAP_WIDTH = Dimensions.get('window').width;
const MAP_HEIGHT = Dimensions.get('window').height - 80;
const MAP_DIMENSIONS = { width: MAP_WIDTH, height: MAP_HEIGHT };

export const Map = () => {
  const [region, setRegion] = useState<Region>(initialRegion);
  const [places, setPlaces] =
    useState<Supercluster.PointFeature<any>[]>(parsedPlacesData);
  const [options, setOptions] = useState({ radius: 18 });
  const mapRef = useRef<MapView>(null);

  const _handlePointPress = (point: IFeature) => {
    if (isClusterFeature(point)) {
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
          data={places}
          region={region}
          options={options}
          mapDimensions={MAP_DIMENSIONS}
          renderItem={(item) => {
            return (
              <Point
                key={
                  isClusterFeature(item)
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
      <View style={styles.buttonContainer}>
        <Button
          title="Add 1000 points"
          onPress={() =>
            setPlaces(parsedPlacesData.concat(getRandomData(1000)))
          }
        />
        <Button
          title="Change options"
          onPress={() => setOptions({ radius: Math.floor(Math.random() * 18) })}
        />
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  buttonContainer: {
    position: 'absolute',
    bottom: 0,
    left: 0,
    right: 0,
    flexDirection: 'row',
    justifyContent: 'space-between',
    width: '100%',
    backgroundColor: '#ed8e8efd',
    borderColor: '#ed8e8e',
    borderWidth: 1,
    borderRadius: 5,
    gap: 10,
    paddingHorizontal: 20,
  },
});

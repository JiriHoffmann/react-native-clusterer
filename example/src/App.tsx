/* eslint-disable react-native/no-inline-styles */
import React, { useState } from 'react';
import {
  StyleSheet,
  View,
  Text,
  TouchableOpacity,
  SafeAreaView,
} from 'react-native';
import { Map } from './Map';
import { Comparison } from './Comparison';
import { Tests } from './Tests';

export default function App() {
  const [showType, setType] = useState<null | 'map' | 'speed' | 'test'>(null);

  return (
    <SafeAreaView style={styles.container}>
      <Text style={styles.header}>Clusterer</Text>
      <View style={styles.buttonContainer}>
        <TouchableOpacity
          style={{
            ...styles.button,
            borderColor: '#ed8e8e',
            backgroundColor: showType === 'map' ? '#ed8e8e' : '#ed8e8e50',
          }}
          onPress={() => setType('map')}
        >
          <Text>🗺️ Map Clustering</Text>
        </TouchableOpacity>
        <TouchableOpacity
          style={{
            ...styles.button,
            borderColor: '#8eedb1',
            backgroundColor: showType === 'speed' ? '#8eedb1' : '#8eedb150',
          }}
          onPress={() => setType('speed')}
        >
          <Text>⚡ Speed Comparison</Text>
        </TouchableOpacity>
        <TouchableOpacity
          style={{
            ...styles.button,
            borderColor: '#8eedb1',
            backgroundColor: showType === 'test' ? '#8eedb1' : '#8eedb150',
          }}
          onPress={() => setType('test')}
        >
          <Text>🧪 Tests</Text>
        </TouchableOpacity>
      </View>
      {showType === 'map' && <Map />}
      {showType === 'speed' && <Comparison />}
      {showType === 'test' && <Tests />}
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
  },
  header: {
    fontSize: 22,
    fontWeight: 'bold',
    marginHorizontal: 10,
    marginBottom: 5,
  },
  buttonContainer: {
    flexDirection: 'row',
    marginBottom: 10,
    marginHorizontal: 10,
    gap: 10,
  },
  button: {
    flex: 1,
    borderRadius: 5,
    justifyContent: 'center',
    alignItems: 'center',
    height: 40,
    borderWidth: 2,
  },
});

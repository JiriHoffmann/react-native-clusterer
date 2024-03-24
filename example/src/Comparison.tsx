/* eslint-disable react-native/no-inline-styles */
import React, { useState, FunctionComponent } from 'react';
import {
  StyleSheet,
  View,
  Text,
  TouchableOpacity,
  TextInput,
} from 'react-native';
import { getRandomData, parsedPlacesData } from './places';
import { GetTile } from './GetTile';
import { GetClusters } from './GetClusters';

const DEFAULT_SIZE = '1000';

export const Comparison: FunctionComponent<{}> = () => {
  const [data, setData] = useState<supercluster.PointFeature<any>[]>(
    getRandomData(DEFAULT_SIZE)
  );
  const [dataSizeInput, setDataSizeInput] = useState(DEFAULT_SIZE);
  const [showType, setType] = useState<null | 'tile' | 'cluster'>(null);

  const _handleDefaultDataPress = () => {
    setData(parsedPlacesData);
  };

  const _handleGenerateDataPress = () => {
    setData(getRandomData(dataSizeInput));
  };

  return (
    <View style={styles.container}>
      <Text style={styles.h2}>Input Data</Text>

      <TouchableOpacity
        style={styles.defaultDataButton}
        onPress={_handleDefaultDataPress}
      >
        <Text>Use supercluster.js test data</Text>
      </TouchableOpacity>
      <Text style={styles.h2}>Or generate random points</Text>
      <View style={styles.inputContainer}>
        <TextInput
          style={{ ...styles.input, marginRight: 10 }}
          placeholder="Enter data size here"
          onChangeText={(t) => setDataSizeInput(t as any)}
          keyboardType={'number-pad'}
          value={`${dataSizeInput}`}
          multiline={false}
        />
        <TouchableOpacity
          style={styles.button}
          onPress={_handleGenerateDataPress}
        >
          <Text>Generate</Text>
        </TouchableOpacity>
      </View>
      <Text style={styles.h2}>Data size: {data.length}</Text>
      <View style={styles.inputContainer}>
        <TouchableOpacity
          style={{
            ...styles.type,
            borderColor: '#9c8eed',
            marginRight: 10,
            backgroundColor: showType === 'tile' ? '#9c8eed' : '#9c8eed50',
          }}
          onPress={() => setType('tile')}
        >
          <Text>getTile</Text>
        </TouchableOpacity>
        <TouchableOpacity
          style={{
            ...styles.type,
            borderColor: '#eda78e',
            backgroundColor: showType === 'cluster' ? '#eda78e' : '#eda78e50',
          }}
          onPress={() => setType('cluster')}
        >
          <Text>getClusters</Text>
        </TouchableOpacity>
      </View>

      {showType === 'tile' && <GetTile data={data} />}
      {showType === 'cluster' && <GetClusters data={data} />}
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    marginHorizontal: 10,
    gap: 10
  },
  h2: {
    fontSize: 16,
    fontWeight: 'bold',
    marginTop: 10,
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
  },
});

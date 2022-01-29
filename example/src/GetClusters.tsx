/* eslint-disable react-native/no-inline-styles */
import React, { FunctionComponent, useState } from 'react';
import {
  StyleSheet,
  SafeAreaView,
  View,
  Text,
  ScrollView,
  TextInput,
  Platform,
  TouchableOpacity,
} from 'react-native';
import { PerformanceNow, superclusterOptions, timeDelta } from './utils';
import Supercluster from 'react-native-clusterer';
import { default as SuperclusterJS } from 'supercluster';

interface Props {
  data: any;
}

const GetClusters: FunctionComponent<Props> = ({ data }) => {
  const [westLng, setWestLng] = useState(-180);
  const [southLat, setSouthLat] = useState(-90);
  const [eastLng, setEastLng] = useState(180);
  const [northLat, setNorthLat] = useState(90);
  const [zoom, setZoom] = useState(1);

  const [time, setTime] = useState<string[]>(['0', '0']);
  const [result, setResult] = useState<string>('');

  const _handleRunJS = () => {
    const initS = PerformanceNow();
    const superclusterJS = new SuperclusterJS(superclusterOptions);
    superclusterJS.load(data);
    const initE = PerformanceNow();

    const getTileS = PerformanceNow();
    const clusterRes = superclusterJS.getClusters(
      [westLng, southLat, eastLng, northLat],
      zoom
    );
    const getTileE = PerformanceNow();
    console.log(JSON.stringify(clusterRes));
    setResult(JSON.stringify(clusterRes));
    setTime([timeDelta(initS, initE), timeDelta(getTileS, getTileE)]);
  };

  const _handleRunCPP = () => {
    const initS = PerformanceNow();
    const supercluster = new Supercluster(data, superclusterOptions);
    const initE = PerformanceNow();

    const getTileS = PerformanceNow();
    const clusterRes = supercluster.getClusters(
      [westLng, southLat, eastLng, northLat],
      zoom
    );
    const getTileE = PerformanceNow();

    supercluster.destroy();

    console.log(JSON.stringify(clusterRes));

    setResult(JSON.stringify(clusterRes));
    setTime([timeDelta(initS, initE), timeDelta(getTileS, getTileE)]);
  };

  return (
    <SafeAreaView style={styles.container}>
      <View style={styles.rowContainer}>
        <Text style={styles.flex}>westLng</Text>
        <Text style={styles.flex}>southLat</Text>
        <Text style={styles.flex}>eastLng</Text>
        <Text style={styles.flex}>northLat</Text>
        <Text style={styles.flex}>zoom</Text>
      </View>
      <View style={styles.rowContainer}>
        <TextInput
          style={styles.flexInput}
          placeholder="0"
          onChangeText={(t) => setWestLng(t as any)}
          keyboardType={'number-pad'}
          value={`${westLng}`}
          multiline={false}
        />
        <TextInput
          style={styles.flexInput}
          placeholder="0"
          onChangeText={(t) => setSouthLat(t as any)}
          keyboardType={'number-pad'}
          value={`${southLat}`}
          multiline={false}
        />
        <TextInput
          style={styles.flexInput}
          placeholder="0"
          onChangeText={(t) => setEastLng(t as any)}
          keyboardType={'number-pad'}
          value={`${eastLng}`}
          multiline={false}
        />
        <TextInput
          style={styles.flexInput}
          placeholder="0"
          onChangeText={(t) => setNorthLat(t as any)}
          keyboardType={'number-pad'}
          value={`${northLat}`}
          multiline={false}
        />
        <TextInput
          style={styles.flexInput}
          placeholder="0"
          onChangeText={(t) => setZoom(t as any)}
          keyboardType={'number-pad'}
          value={`${zoom}`}
          multiline={false}
        />
      </View>

      <View style={styles.buttonContainer}>
        <TouchableOpacity
          style={{ ...styles.button, marginRight: 10 }}
          onPress={_handleRunJS}
        >
          <Text>JS Impementation</Text>
        </TouchableOpacity>
        <TouchableOpacity style={styles.button} onPress={_handleRunCPP}>
          <Text>C++ Impementation</Text>
        </TouchableOpacity>
      </View>

      <Text style={styles.h2}>Initialization time: {time[0]} ms</Text>
      <Text style={styles.h2}>Get clusters time: {time[1]} ms</Text>
      <Text style={styles.h2}>Result:</Text>
      <ScrollView style={styles.scrollView}>
        <Text>{result}</Text>
      </ScrollView>
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  container: {
    paddingTop: 20,
  },
  scrollView: {
    height: 300,
  },
  h2: {
    fontSize: 14,
    fontWeight: 'bold',
  },
  rowContainer: {
    flexDirection: 'row',
    width: '100%',
  },
  flex: {
    flex: 1,
    textAlign: 'center',
  },
  flexInput: {
    flex: 1,
    textAlign: 'center',
    borderWidth: 1,
    borderRadius: 5,
    borderColor: '#eda78e',
    height: 35,
    paddingBottom: Platform.OS === 'android' ? 7 : undefined,
    marginHorizontal: 5,
  },
  buttonContainer: {
    flexDirection: 'row',
  },
  button: {
    flex: 1,
    borderRadius: 5,
    justifyContent: 'center',
    alignItems: 'center',
    height: 40,
    marginVertical: 10,
    backgroundColor: '#eda78e',
  },
});

export { GetClusters };

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

const GetTile: FunctionComponent<Props> = ({ data }) => {
  const [x, setX] = useState(0);
  const [y, setY] = useState(0);
  const [z, setZ] = useState(0);

  const [time, setTime] = useState<string[]>(['0', '0']);
  const [result, setResult] = useState<string>('');

  const _handleRunJS = () => {
    const initS = PerformanceNow();
    const superclusterJS = new SuperclusterJS(superclusterOptions);
    superclusterJS.load(data);
    const initE = PerformanceNow();

    const getTileS = PerformanceNow();
    const tileRes = superclusterJS.getTile(x, y, z);
    const getTileE = PerformanceNow();

    setResult(JSON.stringify(tileRes));
    setTime([timeDelta(initS, initE), timeDelta(getTileS, getTileE)]);
  };

  const _handleRunCPP = () => {
    const initS = PerformanceNow();
    const supercluster = new Supercluster(superclusterOptions);
    supercluster.load(data);
    const initE = PerformanceNow();

    const getTileS = PerformanceNow();
    const tileRes = supercluster.getTile(0, 0, 0);
    const getTileE = PerformanceNow();

    supercluster.destroy();

    setResult(JSON.stringify(tileRes));
    setTime([timeDelta(initS, initE), timeDelta(getTileS, getTileE)]);
  };

  return (
    <SafeAreaView style={styles.container}>
      <View style={styles.rowContainer}>
        <Text style={styles.flex}>x</Text>
        <Text style={styles.flex}>y</Text>
        <Text style={styles.flex}>z</Text>
      </View>
      <View style={styles.rowContainer}>
        <TextInput
          style={styles.flexInput}
          placeholder="0"
          onChangeText={(t) => setX(t as any)}
          keyboardType={'number-pad'}
          value={`${x}`}
          multiline={false}
        />
        <TextInput
          style={styles.flexInput}
          placeholder="0"
          onChangeText={(t) => setY(t as any)}
          keyboardType={'number-pad'}
          value={`${y}`}
          multiline={false}
        />
        <TextInput
          style={styles.flexInput}
          placeholder="0"
          onChangeText={(t) => setZ(t as any)}
          keyboardType={'number-pad'}
          value={`${z}`}
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
      <Text style={styles.h2}>Get tile time: {time[1]} ms</Text>
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
    marginBottom: 10,
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
    borderColor: '#9c8eed',
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
    backgroundColor: '#9c8eed',
  },
});

export { GetTile };

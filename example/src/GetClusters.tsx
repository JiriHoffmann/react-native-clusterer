/* eslint-disable react-native/no-inline-styles */
import { type FunctionComponent, useState } from 'react';
import {
  StyleSheet,
  SafeAreaView,
  View,
  Text,
  ScrollView,
  TextInput,
  Platform,
  TouchableOpacity,
  Modal,
} from 'react-native';
import { PerformanceNow, superclusterOptions, timeDelta } from './utils';
import Supercluster from 'react-native-clusterer';
import SuperclusterJS from 'supercluster';

type BBox = [number, number, number, number];
interface Props {
  data: supercluster.PointFeature<any>[];
}

const GetClusters: FunctionComponent<Props> = ({ data }) => {
  const [westLng, setWestLng] = useState('-180');
  const [southLat, setSouthLat] = useState('-90');
  const [eastLng, setEastLng] = useState('180');
  const [northLat, setNorthLat] = useState('90');
  const [zoom, setZoom] = useState('1');

  const [numberOfRuns, setNumberOfRuns] = useState('100');
  const [jsTimeAverage, setJsTimeAverage] = useState('0');
  const [cppTimeAverage, setCppTimeAverage] = useState('0');

  const zoomInt = parseInt(zoom, 10);
  const bbox = [westLng, southLat, eastLng, northLat].map(parseFloat) as BBox;

  const [time, setTime] = useState<string[]>(['0', '0']);
  const [result, setResult] = useState<string>('');
  const [showModal, setShowModal] = useState<boolean>(false);

  const _handleRunJS = () => {
    if (bbox.some(isNaN)) return console.warn('Invalid input', bbox);
    if (isNaN(zoomInt)) return console.warn('Invalid input', zoomInt);

    const start = PerformanceNow();
    const superclusterJS = new SuperclusterJS(superclusterOptions);
    superclusterJS.load(data);
    const end = PerformanceNow();

    const getClustersS = PerformanceNow();
    const clusterRes = superclusterJS.getClusters(bbox, zoomInt);
    const getClustersE = PerformanceNow();

    setResult(JSON.stringify(clusterRes));
    setTime([timeDelta(start, end), timeDelta(getClustersS, getClustersE)]);
    setShowModal(false); // Don't auto-show modal
  };

  const _handleRunCPP = () => {
    if (bbox.some(isNaN)) return console.warn('Invalid input', bbox);
    if (isNaN(zoomInt)) return console.warn('Invalid input', zoomInt);

    const start = PerformanceNow();
    const supercluster = new Supercluster(superclusterOptions);
    supercluster.load(data);
    const end = PerformanceNow();

    const getClustersS = PerformanceNow();
    const clusterRes = supercluster.getClusters(bbox, zoomInt);
    const getClustersE = PerformanceNow();

    setResult(JSON.stringify(clusterRes));
    setTime([timeDelta(start, end), timeDelta(getClustersS, getClustersE)]);
    setShowModal(false); // Don't auto-show modal
  };

  const _handleRunMultiple = () => {
    const superclusterJS = new SuperclusterJS(superclusterOptions);
    superclusterJS.load(data);

    const supercluster = new Supercluster(superclusterOptions);
    supercluster.load(data);

    const numberOfRunsInt = parseInt(numberOfRuns, 10);

    const randomBboxAndZoom = Array.from({ length: numberOfRunsInt }, () => {
      return [
        Math.random() * 360 - 180,
        Math.random() * 180 - 90,
        Math.random() * 360 - 180,
        Math.random() * 180 - 90,
        Math.floor(Math.random() * 10),
      ];
    });

    const jsTimes: number[] = [];
    const cppTimes: number[] = [];

    for (const bboxAndZoom of randomBboxAndZoom) {
      const startJS = PerformanceNow();
      const clusterResJS = superclusterJS.getClusters(
        bboxAndZoom as BBox,
        bboxAndZoom[4]!
      );
      const endJS = PerformanceNow();

      const startCPP = PerformanceNow();
      const clusterResCPP = supercluster.getClusters(
        bboxAndZoom as BBox,
        bboxAndZoom[4]!
      );
      const endCPP = PerformanceNow();

      if (clusterResJS.length !== clusterResCPP.length) {
        console.warn(
          'size mismatch',
          clusterResJS.length,
          clusterResCPP.length
        );
        continue;
      }

      // only count runs that have a result
      if (clusterResJS.length !== 0) {
        jsTimes.push(parseFloat(timeDelta(startJS, endJS)));
        cppTimes.push(parseFloat(timeDelta(startCPP, endCPP)));
      }
    }

    console.log('datasize', data.length);
    console.log('numberOfRunsInt', numberOfRunsInt);
    console.log('jsTimes', jsTimes);
    console.log('cppTimes', cppTimes);

    setJsTimeAverage(
      `${Math.round((jsTimes.reduce((a, b) => a + b, 0) / numberOfRunsInt) * 1000) / 1000}`
    );
    setCppTimeAverage(
      `${Math.round((cppTimes.reduce((a, b) => a + b, 0) / numberOfRunsInt) * 1000) / 1000}`
    );
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
          onChangeText={setWestLng}
          keyboardType={'number-pad'}
          value={`${westLng}`}
          multiline={false}
        />
        <TextInput
          style={styles.flexInput}
          placeholder="0"
          onChangeText={setSouthLat}
          keyboardType={'number-pad'}
          value={`${southLat}`}
          multiline={false}
        />
        <TextInput
          style={styles.flexInput}
          placeholder="0"
          onChangeText={setEastLng}
          keyboardType={'number-pad'}
          value={`${eastLng}`}
          multiline={false}
        />
        <TextInput
          style={styles.flexInput}
          placeholder="0"
          onChangeText={setNorthLat}
          keyboardType={'number-pad'}
          value={`${northLat}`}
          multiline={false}
        />
        <TextInput
          style={styles.flexInput}
          placeholder="0"
          onChangeText={setZoom}
          keyboardType={'number-pad'}
          value={`${zoom}`}
          multiline={false}
        />
      </View>

      <View style={styles.buttonContainer}>
        <TouchableOpacity style={styles.button} onPress={_handleRunJS}>
          <Text>JS Impementation</Text>
        </TouchableOpacity>
        <TouchableOpacity style={styles.button} onPress={_handleRunCPP}>
          <Text>C++ Impementation</Text>
        </TouchableOpacity>
      </View>

      <View style={styles.resultsContainer}>
        <View>
          <Text>Initialization time: {time[0]} ms</Text>
          <Text>Get clusters time: {time[1]} ms</Text>
        </View>
        <TouchableOpacity
          style={styles.showResultButton}
          onPress={() => setShowModal(true)}
          disabled={!result}
        >
          <Text>Show Result</Text>
        </TouchableOpacity>
      </View>

      <Text style={styles.h2}>Run multiple times</Text>
      <View style={{ flexDirection: 'row', gap: 10, alignItems: 'center' }}>
        <Text style={styles.h3}>Number of runs:</Text>
        <TextInput
          style={styles.flexInput}
          placeholder="100"
          onChangeText={setNumberOfRuns}
          keyboardType={'number-pad'}
          value={`${numberOfRuns}`}
          multiline={false}
        />
      </View>
      <View style={styles.buttonContainer}>
        <TouchableOpacity style={styles.button} onPress={_handleRunMultiple}>
          <Text>Run multiple times</Text>
        </TouchableOpacity>
      </View>

      <Text>JS average time: {jsTimeAverage} ms</Text>
      <Text>C++ average time: {cppTimeAverage} ms</Text>

      <Modal
        visible={showModal}
        transparent={true}
        animationType="fade"
        onRequestClose={() => setShowModal(false)}
      >
        <View
          style={{
            flex: 1,
            backgroundColor: 'rgba(0,0,0,0.4)',
            justifyContent: 'center',
            alignItems: 'center',
          }}
        >
          <View
            style={{
              backgroundColor: 'white',
              padding: 20,
              borderRadius: 10,
              width: '95%',
              height: '75%',
            }}
          >
            <Text style={styles.h3}>Result:</Text>
            <ScrollView style={{ height: 300 }}>
              <Text selectable>{result}</Text>
            </ScrollView>
            <TouchableOpacity
              style={{
                marginTop: 20,
                alignSelf: 'flex-end',
                paddingVertical: 6,
                paddingHorizontal: 15,
                backgroundColor: '#eda78e',
                borderRadius: 5,
              }}
              onPress={() => setShowModal(false)}
            >
              <Text style={{ fontWeight: 'bold' }}>Close</Text>
            </TouchableOpacity>
          </View>
        </View>
      </Modal>
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
    fontSize: 16,
    fontWeight: 'bold',
    marginTop: 10,
    marginBottom: 10,
  },
  h3: {
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
    gap: 10,
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
  showResultButton: {
    backgroundColor: '#eda78e',
    borderRadius: 5,
    paddingVertical: 10,
    paddingHorizontal: 20,
    alignSelf: 'center',
  },
  resultsContainer: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
  },
});

export { GetClusters };

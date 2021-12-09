import React from 'react';
import { useState } from 'react';
import { StyleSheet, View, Text, TouchableOpacity } from 'react-native';
import SuperclusterJS from './superclusterjs';
import SuperclusterCPP from './superclustercpp';


export default function App() {
  const [showMap, setShowMap] = useState<null | "js" | "cpp">(null);

  return (
    <View style={styles.container}>
      <View style={styles.content}>
        {showMap === "js" && <SuperclusterJS />}
        {showMap === "cpp" && <SuperclusterCPP />}
      </View>

      <View style={styles.buttonContainer}>

        <TouchableOpacity style={styles.button} onPress={() => setShowMap("js")}><Text>JS Impementation</Text></TouchableOpacity>
        <TouchableOpacity style={styles.button} onPress={() => setShowMap("cpp")}><Text>C++ Impementation</Text></TouchableOpacity>

      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  content: {
    flex: 1,
    padding: 20
  },
  buttonContainer: {
    position: 'absolute',
    bottom: 20,
    left: 0,
    right: 0,
    flexDirection: 'row',
    justifyContent: 'space-around',
    width: "100%",
  },
  button: {
    flex: 1,
    marginHorizontal: 10,
    borderRadius: 5,
    justifyContent: 'center',
    alignItems: 'center',
    height: 40,
    backgroundColor: '#8eb3ed'
  }
});



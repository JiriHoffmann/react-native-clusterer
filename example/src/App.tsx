import React from 'react';
import { useState } from 'react';
import { StyleSheet, View, Text, TextInput, Button } from 'react-native';
import { multiply } from 'react-native-clusterer';

export default function App() {
  const [input1, setInput1] = useState('');
  const [input2, setInput2] = useState('');

  const [result, setResult] = useState<number | undefined>();

  const _handlePress = () => {
    setResult(multiply(parseInt(input1), parseInt(input2)));
  };

  return (
    <View style={styles.container}>
      <TextInput
        value={input1}
        onChangeText={setInput1}
        style={styles.textInput}
      ></TextInput>
      <TextInput
        value={input2}
        onChangeText={setInput2}
        style={styles.textInput}
      ></TextInput>
      <Button title={'Get Result'} onPress={_handlePress} />
      <Text>Result: {result}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
  box: {
    width: 60,
    height: 60,
    marginVertical: 20,
  },
  textInput: {
    borderColor: 'black',
    borderWidth: 1,
    height: 40,
    width: 200,
    margin: 10,
  },
});

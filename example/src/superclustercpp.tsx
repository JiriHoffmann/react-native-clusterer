import React, { useState } from 'react';
import { StyleSheet, View, Text, Button, ScrollView } from 'react-native';
import { PerformanceNow, superclusterOptions, timeDelta } from './utils';
import { getRandomData } from './places';
import Clusterer from 'react-native-clusterer';

export default function SuperclusterCPP() {
    const [time, setTime] = useState<string[]>(["0", "0"]);
    const [points, setPoints] = useState<string>("");

    const _handleRenderPress = () => {
        const initS = PerformanceNow();
        const supercluster = new Clusterer(getRandomData(1000), superclusterOptions);
        const initE = PerformanceNow();

        const getTileS = PerformanceNow();
        const tileRes = supercluster.getTile(0, 0, 0);
        const getTileE = PerformanceNow();

        supercluster.destroy();

        setPoints(JSON.stringify(tileRes));
        setTime([timeDelta(initS, initE),timeDelta(getTileS, getTileE)])
    }


    return (
        <View style={styles.container}>
            <Text style={styles.header}>Supercluster C++ library</Text>
            <Text style={styles.h2}>Iitialization time: {time[0]} ms</Text>
            <Text style={styles.h2}>Get tile time: {time[1]} ms</Text>
            <Text style={styles.h2}>Tile result:</Text>
            <ScrollView style={styles.scrollView}>
                <Text>{points}</Text>
            </ScrollView>
            <Button title={'Render'} onPress={_handleRenderPress} />

        </View>
    );
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        paddingBottom: 50
    },
    scrollView: {
        marginBottom: 10
    },
    header: {
        fontSize: 20,
        fontWeight: 'bold',
    },
    h2: {
        fontSize: 14,
        fontWeight: 'bold',
    }


});



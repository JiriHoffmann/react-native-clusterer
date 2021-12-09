import React, { useEffect, useState } from 'react';
import { StyleSheet, View, Text, TextInput, Button, ScrollView } from 'react-native';
import { multiply } from 'react-native-clusterer';
import Supercluster from 'supercluster';
import MapView from "react-native-maps"
import { PerformanceNow, superclusterOptions, timeDelta } from './utils';
import { getRandomData } from './places';

const initialRegion = {
    latitude: 37.78825,
    longitude: -122.4324,
    latitudeDelta: 0.0922,
    longitudeDelta: 0.0421
}

export default function SuperclusterJS() {
    const [region, setRegion] = useState(initialRegion);
    const [time, setTime] = useState({ init: 0, getTile: 0, getClusters: 0 });
    const [points, setPoints] = useState([]);

    const _handleRenderPress = () => {
        const initS = PerformanceNow();
        const supercluster = new Supercluster(superclusterOptions);
        supercluster.load(getRandomData(10000));
        const initE = PerformanceNow();

        const getTileS = PerformanceNow();
        const tileRes = supercluster.getTile(0, 0, 0);
        const getTileE = PerformanceNow();

        setPoints(tileRes);
        setTime({
            init: timeDelta(initS, initE),
            getTile: timeDelta(getTileS, getTileE),
            getClusters: 0
        })
    }


    return (
        <View style={styles.container}>
            <Text style={styles.header}>Supercluster JS library</Text>
            <Text  style={styles.h2}>Initialization time: {time.init} ms</Text>
            <Text style={styles.h2}>Get tile time: {time.getTile} ms</Text>
            <Text style={styles.h2}>Tile result:</Text>
            <ScrollView style={styles.scrollView}> 

            <Text>{JSON.stringify(points)}</Text>
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
    scrollView:{
        marginBottom: 10
    },
    header: {
        fontSize: 20,
        fontWeight: 'bold',
    },
    h2:{
        fontSize: 14,
        fontWeight: 'bold',
    }
    

});



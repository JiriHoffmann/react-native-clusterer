#ifndef CLUSTERER_H
#define CLUSTERER_H

#include <map>
#include <jsi/jsi.h>

#include "supercluster.hpp"

using namespace std;
using namespace facebook;

void cluster_init(jsi::Runtime &rt, jsi::Value const &nVal, jsi::Value const &fVal, jsi::Value const &oVal);

jsi::Array cluster_getTile(jsi::Runtime &rt, const string& name, int zoom, int x, int y);

jsi::Array cluster_getClusters(jsi::Runtime &rt, const string& name, double bbox[4], int zoom);

jsi::Array cluster_getChildren(jsi::Runtime &rt, const string& name, int cluster_id);

jsi::Array cluster_getLeaves(jsi::Runtime &rt, const string& name, int cluster_id, int limit, int offset);

int cluster_getClusterExpansionZoom(const string& name, int cluster_id);

void cluster_destroyCluster(const string& name);

void cluster_cleanup();

/*

Helper functions

*/

mapbox::supercluster::Options parseJSIOptions(jsi::Runtime &rt, jsi::Value const &value);

mapbox::feature::feature<double> parseJSIFeature(jsi::Runtime &rt, jsi::Value const &value);

mapbox::feature::feature_collection<double> parseJSIFeatures(jsi::Runtime &rt, jsi::Value const &value);

jsi::Object featureToJSIObject(jsi::Runtime &rt, mapbox::feature::feature<double> &f, bool geometryAsInt);

jsi::Object clusterFeatureToJSIObject(jsi::Runtime &rt, mapbox::feature::feature<double> &f);

jsi::Object propertiesToJSIObject(jsi::Runtime &rt, mapbox::feature::feature<double> &f);

#endif // CLUSTERER_H

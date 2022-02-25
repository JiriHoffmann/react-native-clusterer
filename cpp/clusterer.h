#ifndef CLUSTERER_H
#define CLUSTERER_H

#include <map>
#include <jsi/jsi.h>

#include "supercluster.hpp"

using namespace std;
using namespace facebook;

void cluster_load(jsi::Runtime &rt, jsi::Value const &jsiID, jsi::Value const &jsiFeatures, jsi::Value const &jsiOptions);

jsi::Array cluster_getTile(jsi::Runtime &rt, const string &id, int zoom, int x, int y);

jsi::Array cluster_getClusters(jsi::Runtime &rt, const string &id, double bbox[4], int zoom);

jsi::Array cluster_getChildren(jsi::Runtime &rt, const string &id, int cluster_id);

jsi::Array cluster_getLeaves(jsi::Runtime &rt, const string &id, int cluster_id, int limit, int offset);

int cluster_getClusterExpansionZoom(const string &id, int cluster_id);

bool cluster_destroyCluster(const string &id);

void cluster_cleanup();

/*

Helper functions

*/
mapbox::supercluster::Supercluster *getSuperclusterFromMap(const string &id);

void parseJSIFeatures(jsi::Runtime &rt, mapbox::feature::feature_collection<double> &features, jsi::Value const &jsiFeatures);

void parseJSIOptions(jsi::Runtime &rt, mapbox::supercluster::Options &options, jsi::Value const &jsiOptions);

void parseJSIFeature(jsi::Runtime &rt, mapbox::feature::feature<double> &feature, jsi::Value const &jsiFeature);

void clusterToJSI(jsi::Runtime &rt, jsi::Object &jsiObject, mapbox::feature::feature<double> &f);

void featureToJSI(jsi::Runtime &rt, jsi::Object &jsiObject, mapbox::feature::feature<std::int16_t> &f);

void propertiesToJSI(jsi::Runtime &rt, jsi::Object &jsiObject, mapbox::feature::feature<double> &f);

void propertiesToJSI(jsi::Runtime &rt, jsi::Object &jsiObject, mapbox::feature::feature<std::int16_t> &f);

#endif // CLUSTERER_H

#ifndef CLUSTERER_H
#define CLUSTERER_H

#include <map>
#include <jsi/jsi.h>

#include "supercluster.hpp"


using namespace std;
using namespace facebook;


void cluster_init(string name, jsi::Runtime &rt, jsi::Value const &v1, jsi::Value const &v2);

jsi::Array cluster_getTile(string name, jsi::Runtime &rt, int zoom, int x, int y);

jsi::Array cluster_getChildren(string name,  jsi::Runtime &rt, int cluster_id);

jsi::Array cluster_getLeaves(string name, jsi::Runtime &rt, int cluster_id, int limit, int offset);

int cluster_getClusterExpansionZoom(string name, int cluster_id);

mapbox::supercluster::Options cluster_parseJSIOptions(jsi::Runtime &rt, jsi::Value const &value);

mapbox::feature::feature<double> parseJSIFeature(jsi::Runtime &rt, jsi::Value const &value);

mapbox::feature::feature_collection<double> cluster_parseJSIFeatures(jsi::Runtime &rt, jsi::Value const &value);

jsi::Object tileToJSIObject(jsi::Runtime &rt, mapbox::feature::feature<double> &f, bool geometryAsInt);

#endif //CLUSTERER_CPP_H

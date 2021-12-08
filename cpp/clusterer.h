#ifndef CLUSTERER_H
#define CLUSTERER_H

#include <map>
#include <jsi/jsi.h>

#include "supercluster.hpp"


using namespace std;
using namespace facebook;


void cluster_init(string name, jsi::Runtime &rt, jsi::Value const &v1, jsi::Value const &v2);

mapbox::supercluster::TileFeatures cluster_getTile(string name, int x, int y, int z);

mapbox::supercluster::GeoJSONFeatures cluster_getChildren(string name, int cluster_id);

mapbox::supercluster::GeoJSONFeatures cluster_getLeaves(string name, int cluster_id, int limit, int offset);

int cluster_getClusterExpansionZoom(string name, int cluster_id);

mapbox::supercluster::Options cluster_parseJSIOptions(jsi::Runtime &rt, jsi::Value const &value);

mapbox::feature::feature<double> parseJSIFeature(jsi::Runtime &rt, jsi::Value const &value);

mapbox::feature::feature_collection<double> cluster_parseJSIFeatures(jsi::Runtime &rt, jsi::Value const &value);



#endif //CLUSTERER_CPP_H

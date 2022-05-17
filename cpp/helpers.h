#pragma once

#include <jsi/jsi.h>
#include <map>

#include "supercluster.hpp"

using namespace std;
using namespace facebook;

namespace clusterer {

void parseJSIFeatures(jsi::Runtime &rt,
                      mapbox::feature::feature_collection<double> &features,
                      jsi::Value const &jsiFeatures);

void parseJSIOptions(jsi::Runtime &rt, mapbox::supercluster::Options &options,
                     jsi::Value const &jsiOptions);

void parseJSIFeature(jsi::Runtime &rt,
                     mapbox::feature::feature<double> &feature,
                     jsi::Value const &jsiFeature);

void clusterToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
                  mapbox::feature::feature<double> &f);

void featureToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
                  mapbox::feature::feature<std::int16_t> &f);

void propertiesToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
                     mapbox::feature::feature<double> &f);

void propertiesToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
                     mapbox::feature::feature<std::int16_t> &f);

} // namespace clusterer
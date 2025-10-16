#pragma once

#include <jsi/jsi.h>

#include <map>

#include "supercluster.hpp"

using namespace std;
using namespace facebook;

namespace margelo::nitro::clusterer {

void parseJSIOptions(jsi::Runtime &rt, mapbox::supercluster::Options &options,
                     jsi::Value const &jsiOptions);

void parseJSIFeature(jsi::Runtime &rt, int featureIndex,
                     mapbox::feature::feature<double> &feature,
                     jsi::Value const &jsiFeature);

void clusterToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
                  mapbox::feature::feature<double> &f,
                  jsi::Array &featuresInput);

void tileToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
               mapbox::feature::feature<std::int16_t> &f,
               jsi::Array &featuresInput);

void featurePropertyToJSI(jsi::Runtime &rt,
                   jsi::Object &jsiFeatureProperties,
                   std::pair<const std::string, mapbox::feature::value> &itr,
                   int &origFeatureIndex);

}  // namespace clusterer

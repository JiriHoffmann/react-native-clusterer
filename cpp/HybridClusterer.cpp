#include "HybridClusterer.hpp"

#include "jsiHelpers.hpp"

namespace margelo::nitro::clusterer {
jsi::Value HybridClusterer::load(jsi::Runtime &rt, const jsi::Value &_,
                                 const jsi::Value *args, size_t count) {
  if(count != 2)
    throw jsi::JSError(rt, "React-Native-Clusterer: expects 2 arguments");

  // jsi features to cpp
  mapbox::feature::feature_collection<double> features;

  if(args[0].isObject() && args[0].asObject(rt).isArray(rt)) {
    featuresInput = args[0].asObject(rt).asArray(rt);
    for(int i = 0; i < featuresInput->size(rt); i++) {
      mapbox::feature::feature<double> feature;
      parseJSIFeature(rt, i, feature, featuresInput->getValueAtIndex(rt, i));
      features.push_back(feature);
    }
  } else {
    throw jsi::JSError(rt, "Expected array of GeoJSON Feature objects");
  }
  // jsi options to cpp
  mapbox::supercluster::Options options;
  parseJSIOptions(rt, options, args[1]);

  try {
    instance = new mapbox::supercluster::Supercluster(features, options);
  } catch(exception &e) {
    std::string message =
        std::string("React-Native-Clusterer: Error creating Supercluser") +
        e.what();
    throw jsi::JSError(rt, message.c_str());
  }

  return jsi::Value();
}

jsi::Value HybridClusterer::getClusters(jsi::Runtime &rt,
                                        const jsi::Value &thisValue,
                                        const jsi::Value *args, size_t count) {
  if(count != 2 || !args[0].asObject(rt).isArray(rt) || !args[1].isNumber())
    throw jsi::JSError(rt,
                       "React-Native-Clusterer: getClusters "
                       "expects an array and a number");

  double bbox[4];

  try {
    auto jsibbox = args[0].asObject(rt).asArray(rt);
    bbox[0] = jsibbox.getValueAtIndex(rt, 0).asNumber();
    bbox[1] = jsibbox.getValueAtIndex(rt, 1).asNumber();
    bbox[2] = jsibbox.getValueAtIndex(rt, 2).asNumber();
    bbox[3] = jsibbox.getValueAtIndex(rt, 3).asNumber();
  } catch(exception &e) {
    throw jsi::JSError(rt,
                       "React-Native-Clusterer: GetClusters error, make sure "
                       "boundingBox is an array of 4 numbers");
  }

  int zoom = (int)args[1].asNumber();

  auto clusters = instance.value()->getClusters(bbox, zoom);
  jsi::Array result = jsi::Array(rt, clusters.size());

  int i = 0;
  for(auto &cluster : clusters) {
    jsi::Object jsiCluster = jsi::Object(rt);
    clusterToJSI(rt, jsiCluster, cluster, featuresInput.value());
    result.setValueAtIndex(rt, i, jsiCluster);
    i++;
  }
  return result;
}

jsi::Value HybridClusterer::getTile(jsi::Runtime &rt,
                                        const jsi::Value &thisValue,
                                        const jsi::Value *args, size_t count) {
  if(count != 3 || !args[0].isNumber() || !args[1].isNumber() ||
              !args[2].isNumber())
             throw jsi::JSError(rt,
                                "React-Native-Clusterer: getTile "
                                "expects 3 numbers as arguments");

           int zoom = (int)args[0].asNumber();
           int x = (int)args[1].asNumber();
           int y = (int)args[2].asNumber();

           auto tiles = instance.value()->getTile(zoom, x, y);

           jsi::Array result = jsi::Array(rt, tiles.size());
           int i = 0;
           for(auto &tile : tiles) {
             jsi::Object jsiTile = jsi::Object(rt);
             tileToJSI(rt, jsiTile, tile, featuresInput.value());
             result.setValueAtIndex(rt, i, jsiTile);
             i++;
           }
           return result;
}

jsi::Value HybridClusterer::getChildren(jsi::Runtime &rt,
                                        const jsi::Value &thisValue,
                                        const jsi::Value *args, size_t count) {
  if(count != 1 || !args[0].isNumber())
    throw jsi::JSError(rt,
                       "React-Native-Clusterer: getChildren "
                       "expects a number for cluster_id");

  auto cluster_id = (int)args[0].asNumber();
  auto children = instance.value()->getChildren(cluster_id);
  jsi::Array result = jsi::Array(rt, children.size());

  int i = 0;
  for(auto &child : children) {
    jsi::Object jsiChild = jsi::Object(rt);
    clusterToJSI(rt, jsiChild, child, featuresInput.value());
    result.setValueAtIndex(rt, i, jsiChild);
    i++;
  }
  return result;
}

jsi::Value HybridClusterer::getLeaves(jsi::Runtime &rt,
                                      const jsi::Value &thisValue,
                                      const jsi::Value *args, size_t count) {
  if(count < 1 || count > 3)
    throw jsi::JSError(rt,
                       "React-Native-Clusterer: getLeaves "
                       "expects at least 1 argument, at most 3");

  if(!args[0].isNumber())
    throw jsi::JSError(rt,
                       "React-Native-Clusterer: getLeaves "
                       "first argument must be a number");

  if(count >= 2 && !args[1].isNumber())
    throw jsi::JSError(rt,
                       "React-Native-Clusterer: getLeaves "
                       "second argument must be a number");

  if(count == 3 && !args[2].isNumber())
    throw jsi::JSError(rt,
                       "React-Native-Clusterer: getLeaves "
                       "third argument must be a number");

  auto cluster_id = (int)args[0].asNumber();
  auto limit = count >= 2 ? (int)args[1].asNumber() : 10;
  auto offset = count == 3 ? (int)args[2].asNumber() : 0;

  auto leaves = instance.value()->getLeaves(cluster_id, limit, offset);
  jsi::Array result = jsi::Array(rt, leaves.size());

  int i = 0;
  for(auto &leaf : leaves) {
    jsi::Object jsiLeaf = jsi::Object(rt);
    clusterToJSI(rt, jsiLeaf, leaf, featuresInput.value());
    result.setValueAtIndex(rt, i, jsiLeaf);
    i++;
  }

  return result;
}

jsi::Value HybridClusterer::getClusterExpansionZoom(jsi::Runtime &rt,
                                                    const jsi::Value &thisValue,
                                                    const jsi::Value *args,
                                                    size_t count) {
  if(count != 1 || !args[0].isNumber())
    throw jsi::JSError(
        rt,
        "React-Native-Clusterer: getClusterExpansionZoom expects "
        "number for cluster_id");

  auto cluster_id = (int)args[0].asNumber();

  return (int)instance.value()->getClusterExpansionZoom(cluster_id);
}

}  // namespace margelo::nitro::clusterer

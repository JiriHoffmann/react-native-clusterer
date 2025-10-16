#pragma once

#include "HybridClustererSpec.hpp"
#include "supercluster.hpp"

namespace margelo::nitro::clusterer {
class HybridClusterer final : public HybridClustererSpec {
 public:
  HybridClusterer() : HybridObject(TAG) {}
  ~HybridClusterer() {
    auto instancePtr = instance.value();
    delete instancePtr;
  }

 public:
  jsi::Value load(jsi::Runtime &runtime, const jsi::Value &thisValue,
                  const jsi::Value *args, size_t count);
  jsi::Value getClusters(jsi::Runtime &runtime, const jsi::Value &thisValue,
                         const jsi::Value *args, size_t count);
  jsi::Value getTile(jsi::Runtime &rt,
                                          const jsi::Value &thisValue,
                     const jsi::Value *args, size_t count);
  jsi::Value getChildren(jsi::Runtime &rt,
                                          const jsi::Value &thisValue,
                                          const jsi::Value *args, size_t count);
  jsi::Value getLeaves(jsi::Runtime &rt,
                                                   const jsi::Value &thisValue,
                                                   const jsi::Value *args,
                                                   size_t count);
  jsi::Value getClusterExpansionZoom(
      jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args,
      size_t count);

  void loadHybridMethods() override {
    // register base prototype
    HybridClustererSpec::loadHybridMethods();
    // register all methods we override here
    registerHybrids(this, [](Prototype &prototype) {
      prototype.registerRawHybridMethod("load", 0, &HybridClusterer::load);
      prototype.registerRawHybridMethod("getClusters", 0,
                                        &HybridClusterer::getClusters);
      prototype.registerRawHybridMethod("getTile", 0,
                                        &HybridClusterer::getTile);
      prototype.registerRawHybridMethod("getChildren", 0,
                                        &HybridClusterer::getChildren);
      prototype.registerRawHybridMethod("getLeaves", 0,
                                        &HybridClusterer::getLeaves);
      prototype.registerRawHybridMethod("getClusterExpansionZoom", 0,
                                        &HybridClusterer::getClusterExpansionZoom);
    });
  }

 private:
  std::optional<mapbox::supercluster::Supercluster *> instance = std::nullopt;
  std::optional<jsi::Array> featuresInput = std::nullopt;
};

}  // namespace margelo::nitro::clusterer

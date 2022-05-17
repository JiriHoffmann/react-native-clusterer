#pragma once

#include "helpers.h"
#include "supercluster.hpp"
#include <jsi/jsi.h>

using namespace std;
using namespace facebook;

namespace clusterer {
class JSI_EXPORT SuperclusterHostObject : public jsi::HostObject {
public:
  SuperclusterHostObject(jsi::Runtime &rt, const jsi::Value *args,
                         size_t count);
  ~SuperclusterHostObject();

public:
  jsi::Value get(jsi::Runtime &, const jsi::PropNameID &name) override;
  std::vector<jsi::PropNameID> getPropertyNames(jsi::Runtime &rt) override;

private:
  mapbox::supercluster::Supercluster *instance;
};
} // namespace clusterer
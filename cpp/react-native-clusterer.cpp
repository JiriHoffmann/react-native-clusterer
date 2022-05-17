#include "react-native-clusterer.h"

namespace clusterer {
void install(jsi::Runtime &jsiRuntime) {

  auto createSupercluster = jsi::Function::createFromHostFunction(
      jsiRuntime, jsi::PropNameID::forAscii(jsiRuntime, "createSupercluster"),
      2,
      [](jsi::Runtime &rt, const jsi::Value &thisVal, const jsi::Value *args,
         size_t count) -> jsi::Value {
        auto instance =
            std::make_shared<SuperclusterHostObject>(rt, args, count);

        return jsi::Object::createFromHostObject(rt, instance);
      });
  jsiRuntime.global().setProperty(jsiRuntime, "createSupercluster",
                                  std::move(createSupercluster));
}
} // namespace clusterer

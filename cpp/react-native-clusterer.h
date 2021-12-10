#ifndef REACT_NATIVE_CLUSTERER_H
#define REACT_NATIVE_CLUSTERER_H

#include <jsi/jsilib.h>
#include <jsi/jsi.h>

namespace clusterer
{

  void install(facebook::jsi::Runtime &jsiRuntime);
  void cleanup();
}

#endif /* REACT_NATIVE_CLUSTERER_H */

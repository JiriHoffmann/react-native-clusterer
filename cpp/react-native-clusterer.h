#ifndef REACT_NATIVE_CLUSTERER_H
#define REACT_NATIVE_CLUSTERER_H

#include <jsi/jsilib.h>
#include <jsi/jsi.h>

#include "clusterer.h"

using namespace facebook;
using namespace std;

namespace clusterer
{

  void install(jsi::Runtime &jsiRuntime);
  void cleanup();
}

#endif /* REACT_NATIVE_CLUSTERER_H */

#pragma once

#include <jsi/jsi.h>
#include <jsi/jsilib.h>

#include "SuperclusterHostObject.h"

using namespace facebook;
using namespace std;

namespace clusterer {

void install(jsi::Runtime &jsiRuntime);
void cleanup();
} // namespace clusterer
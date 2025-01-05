#include "helpers.h"

namespace clusterer {

void parseJSIOptions(jsi::Runtime &rt, mapbox::supercluster::Options &options,
                     jsi::Value const &jsiOptions) {
  if(jsiOptions.isObject()) {
    jsi::Object obj = jsiOptions.asObject(rt);

    if(obj.hasProperty(rt, "radius")) {
      jsi::Value radius = obj.getProperty(rt, "radius");
      if(radius.isNumber()) {
        options.radius = (int)radius.asNumber();
      } else
        throw jsi::JSError(rt, "Expected number for radius");
    }

    if(obj.hasProperty(rt, "minZoom")) {
      jsi::Value minZoom = obj.getProperty(rt, "minZoom");
      if(minZoom.isNumber()) {
        options.minZoom = (int)minZoom.asNumber();
      } else
        throw jsi::JSError(rt, "Expected number for minZoom");
    }

    if(obj.hasProperty(rt, "maxZoom")) {
      jsi::Value maxZoom = obj.getProperty(rt, "maxZoom");
      if(maxZoom.isNumber()) {
        options.maxZoom = (int)maxZoom.asNumber();
      } else
        throw jsi::JSError(rt, "Expected number for maxZoom");
    }

    if(obj.hasProperty(rt, "extent")) {
      jsi::Value extent = obj.getProperty(rt, "extent");
      if(extent.isNumber()) {
        options.extent = (int)extent.asNumber();
      } else
        throw jsi::JSError(rt, "Expected number for extent");
    }
    if(obj.hasProperty(rt, "minPoints")) {
      jsi::Value minPoints = obj.getProperty(rt, "minPoints");
      if(minPoints.isNumber()) {
        options.minPoints = (int)minPoints.asNumber();
      } else
        throw jsi::JSError(rt, "Expected number for minPoints");
    }
    if(obj.hasProperty(rt, "generateId")) {
      jsi::Value generateId = obj.getProperty(rt, "generateId");
      if(generateId.isBool()) {
        options.generateId = generateId.getBool();
      } else
        throw jsi::JSError(rt, "Expected boolean for generateId");
    }
  } else
    throw jsi::JSError(rt, "Expected object for options");
};

void parseJSIFeature(jsi::Runtime &rt, int featureIndex,
                     mapbox::feature::feature<double> &feature,
                     jsi::Value const &jsiFeature) {
  if(!jsiFeature.isObject())
    throw jsi::JSError(rt, "Expected GeoJSON Feature object");

  jsi::Object obj = jsiFeature.asObject(rt);

  // obj.type
  if(!obj.hasProperty(rt, "type") ||
     !strcmp(obj.getProperty(rt, "type").asString(rt).utf8(rt).c_str(),
             "Point"))
    throw jsi::JSError(rt, "Expected GeoJSON Feature object with type 'Point'");

  // obj.geometry
  if(!obj.hasProperty(rt, "geometry"))
    throw jsi::JSError(rt, "Expected geometry object");

  jsi::Value geometry = obj.getProperty(rt, "geometry");

  if(!geometry.isObject()) throw jsi::JSError(rt, "Expected geometry object");

  jsi::Object geoObj = geometry.asObject(rt);

  // obj.geometry.coordinates
  if(!geoObj.hasProperty(rt, "coordinates"))
    throw jsi::JSError(rt, "Expected coordinates property");

  jsi::Value coordinates = geoObj.getProperty(rt, "coordinates");

  if(!coordinates.asObject(rt).isArray(rt))
    throw jsi::JSError(rt, "Expected array for coordinates");

  jsi::Array arr = coordinates.asObject(rt).asArray(rt);

  if(arr.size(rt) != 2)
    throw jsi::JSError(rt, "Expected array of size 2 for coordinates");

  jsi::Value x = arr.getValueAtIndex(rt, 0);
  jsi::Value y = arr.getValueAtIndex(rt, 1);

  if(!x.isNumber() || !y.isNumber())
    throw jsi::JSError(rt, "Expected number for coordinates");

  double lng = x.asNumber();
  double lat = y.asNumber();
  mapbox::geometry::point<double> point(lng, lat);
  feature.geometry = point;

  feature.properties["_clusterer_index"] = std::uint64_t(featureIndex);
};

void clusterToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
                  mapbox::feature::feature<double> &f,
                  jsi::Array &featuresInput) {
  // .id
  if(f.id.is<uint64_t>()) {
    jsiObject.setProperty(rt, "id", jsi::Value((int)f.id.get<uint64_t>()));
  }

  //  .type
  jsiObject.setProperty(rt, "type", jsi::String::createFromUtf8(rt, "Feature"));

  // .geometry - differs from tile geometry
  jsi::Object geometry = jsi::Object(rt);
  jsi::Array coordinates = jsi::Array(rt, 2);
  auto geo = f.geometry.get<mapbox::geometry::point<double>>();
  coordinates.setValueAtIndex(rt, 0, jsi::Value(geo.x));
  coordinates.setValueAtIndex(rt, 1, jsi::Value(geo.y));
  geometry.setProperty(rt, "type", jsi::String::createFromUtf8(rt, "Point"));
  geometry.setProperty(rt, "coordinates", coordinates);
  jsiObject.setProperty(rt, "geometry", geometry);

  // .properties
  jsi::Object properties = jsi::Object(rt);
  int origFeatureIndex = -1;
  for(auto &itr : f.properties) {
    featurePropertyToJSI(rt, properties, itr, origFeatureIndex);
  }

  if(origFeatureIndex != -1) {
    jsi::Object originalFeature =
        featuresInput.getValueAtIndex(rt, origFeatureIndex).asObject(rt);
    if(originalFeature.hasProperty(rt, "properties")) {
      jsiObject.setProperty(rt, "properties",
                            originalFeature.getProperty(rt, "properties"));
    }
  } else {
    jsiObject.setProperty(rt, "properties", properties);
  }
}

void tileToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
               mapbox::feature::feature<std::int16_t> &f,
               jsi::Array &featuresInput) {
  // .id
  if(f.id.is<uint64_t>()) {
    jsiObject.setProperty(rt, "id", jsi::Value((int)f.id.get<uint64_t>()));
  }

  // .type
  jsiObject.setProperty(rt, "type", 1);

  // .geometry
  jsi::Array geometryContainer = jsi::Array(rt, 1);
  jsi::Array geometry = jsi::Array(rt, 2);
  auto geo = f.geometry.get<mapbox::geometry::point<std::int16_t>>();
  geometry.setValueAtIndex(rt, 0, jsi::Value((int)geo.x));
  geometry.setValueAtIndex(rt, 1, jsi::Value((int)geo.y));
  geometryContainer.setValueAtIndex(rt, 0, geometry);
  jsiObject.setProperty(rt, "geometry", geometryContainer);

  // .tags
  jsi::Object tags = jsi::Object(rt);
  int origFeatureIndex = -1;
  for(auto &itr : f.properties) {
    featurePropertyToJSI(rt, tags, itr, origFeatureIndex);
  }

  if(origFeatureIndex != -1) {
    jsi::Object originalFeature =
        featuresInput.getValueAtIndex(rt, origFeatureIndex).asObject(rt);
    if(originalFeature.hasProperty(rt, "properties")) {
      jsiObject.setProperty(rt, "tags",
                            originalFeature.getProperty(rt, "properties"));
    }
  } else {
    jsiObject.setProperty(rt, "tags", tags);
  }
}

void featurePropertyToJSI(
    jsi::Runtime &rt, jsi::Object &jsiFeatureProperties,
    std::pair<const std::string, mapbox::feature::value> &itr,
    int &origFeatureIndex) {
  auto name = itr.first;
  auto nameJSI = jsi::String::createFromUtf8(rt, name);
  auto type = itr.second.which();

  if(name == "_clusterer_index") {
    origFeatureIndex = (int)itr.second.get<std::uint64_t>();
  }
  // Boolean
  else if(type == 1) {
    jsiFeatureProperties.setProperty(rt, nameJSI,
                                     jsi::Value(itr.second.get<bool>() == 1));
  }
  // Integer
  else if(type == 2) {
    jsiFeatureProperties.setProperty(rt, nameJSI,
                                     (int)itr.second.get<std::uint64_t>());
  }
  // Double
  else if(type == 3 || type == 4) {
    jsiFeatureProperties.setProperty(rt, nameJSI,
                                     jsi::Value(itr.second.get<double>()));
  }
  // String
  else if(type == 5) {
    jsiFeatureProperties.setProperty(
        rt, nameJSI,
        jsi::String::createFromUtf8(rt, itr.second.get<std::string>()));
    return;
  }
}

double calculateDelta(double x, double y) {
  if(x > y) {
    return x - y;
  }
  return y - x;
}

double calculateAverage(initializer_list<double> args) {
  if(args.size() == 0) {
    return 0;
  }

  double sum = 0;
  for(auto &num : args) sum += num;

  return sum / args.size();
}

void installHelpers(jsi::Runtime &jsiRuntime) {
  auto regionToBBox = jsi::Function::createFromHostFunction(
      jsiRuntime, jsi::PropNameID::forAscii(jsiRuntime, "regionToBBox"), 1,
      [](jsi::Runtime &runtime, const jsi::Value &thisValue,
         const jsi::Value *arguments, size_t count) -> jsi::Array {
        jsi::Object region = arguments[0].getObject(runtime);

        double longitudeDelta =
            region.getProperty(runtime, "longitudeDelta").asNumber();

        double latitudeDelta =
            region.getProperty(runtime, "latitudeDelta").asNumber();

        double longitude = region.getProperty(runtime, "longitude").asNumber();

        double latitude = region.getProperty(runtime, "latitude").asNumber();

        double lngD = longitudeDelta;

        if(longitudeDelta < 0) {
          lngD = longitudeDelta + 360;
        }

        jsi::Array bbox = jsi::Array(runtime, 4);

        bbox.setValueAtIndex(runtime, 0, longitude - lngD);
        bbox.setValueAtIndex(runtime, 1, latitude - latitudeDelta);
        bbox.setValueAtIndex(runtime, 2, longitude + lngD);
        bbox.setValueAtIndex(runtime, 3, latitude + latitudeDelta);

        return bbox;
      });

  auto getMarkersRegion = jsi::Function::createFromHostFunction(
      jsiRuntime, jsi::PropNameID::forAscii(jsiRuntime, "getMarkersRegion"), 1,
      [](jsi::Runtime &runtime, const jsi::Value &thisValue,
         const jsi::Value *arguments, size_t count) -> jsi::Object {
        auto points = arguments[0].getObject(runtime).asArray(runtime);

        jsi::Object initialValue =
            points.getValueAtIndex(runtime, 0).asObject(runtime);

        jsi::Object coordinates = jsi::Object(runtime);

        coordinates.setProperty(
            runtime, "minX",
            initialValue.getProperty(runtime, "latitude").asNumber());

        coordinates.setProperty(
            runtime, "maxX",
            initialValue.getProperty(runtime, "latitude").asNumber());

        coordinates.setProperty(
            runtime, "minY",
            initialValue.getProperty(runtime, "longitude").asNumber());

        coordinates.setProperty(
            runtime, "maxY",
            initialValue.getProperty(runtime, "longitude").asNumber());

        for(int i = 0; i < points.size(runtime); i++) {
          jsi::Object point =
              points.getValueAtIndex(runtime, i).asObject(runtime);

          double minX =
              std::min(coordinates.getProperty(runtime, "minX").asNumber(),
                       point.getProperty(runtime, "latitude").asNumber());

          double maxX =
              std::max(coordinates.getProperty(runtime, "maxX").asNumber(),
                       point.getProperty(runtime, "latitude").asNumber());

          double minY =
              std::min(coordinates.getProperty(runtime, "minY").asNumber(),
                       point.getProperty(runtime, "longitude").asNumber());

          double maxY =
              std::max(coordinates.getProperty(runtime, "maxY").asNumber(),
                       point.getProperty(runtime, "longitude").asNumber());

          coordinates.setProperty(runtime, "minX", minX);

          coordinates.setProperty(runtime, "maxX", maxX);

          coordinates.setProperty(runtime, "minY", minY);

          coordinates.setProperty(runtime, "maxY", maxY);
        }

        double deltaX =
            calculateDelta(coordinates.getProperty(runtime, "maxX").asNumber(),
                           coordinates.getProperty(runtime, "minX").asNumber());
        double deltaY =
            calculateDelta(coordinates.getProperty(runtime, "maxY").asNumber(),
                           coordinates.getProperty(runtime, "minY").asNumber());

        jsi::Object region = jsi::Object(runtime);

        region.setProperty(
            runtime, "latitude",
            calculateAverage(
                {coordinates.getProperty(runtime, "minX").asNumber(),
                 coordinates.getProperty(runtime, "maxX").asNumber()}));

        region.setProperty(
            runtime, "longitude",
            calculateAverage(
                {coordinates.getProperty(runtime, "minY").asNumber(),
                 coordinates.getProperty(runtime, "maxY").asNumber()}));

        region.setProperty(runtime, "latitudeDelta", deltaX * 1.5);
        region.setProperty(runtime, "longitudeDelta", deltaY * 1.5);

        return region;
      });

  jsiRuntime.global().setProperty(jsiRuntime, "regionToBBox",
                                  std::move(regionToBBox));
  jsiRuntime.global().setProperty(jsiRuntime, "getMarkersRegion",
                                  std::move(getMarkersRegion));
}
}  // namespace clusterer

#include "helpers.h"

namespace clusterer {

void parseJSIFeatures(jsi::Runtime &rt,
                      mapbox::feature::feature_collection<double> &features,
                      jsi::Value const &jsiFeatures) {
  if (jsiFeatures.asObject(rt).isArray(rt)) {
    jsi::Array arr = jsiFeatures.asObject(rt).asArray(rt);
    for (int i = 0; i < arr.size(rt); i++) {
      mapbox::feature::feature<double> feature;
      parseJSIFeature(rt, feature, arr.getValueAtIndex(rt, i));
      features.push_back(feature);
    }
  } else {
    throw jsi::JSError(rt, "Expected array of GeoJSON Feature objects");
  }
};

void parseJSIOptions(jsi::Runtime &rt, mapbox::supercluster::Options &options,
                     jsi::Value const &jsiOptions) {
  if (jsiOptions.isObject()) {
    jsi::Object obj = jsiOptions.asObject(rt);

    if (obj.hasProperty(rt, "radius")) {
      jsi::Value radius = obj.getProperty(rt, "radius");
      if (radius.isNumber()) {
        options.radius = (int)radius.asNumber();
      } else
        throw jsi::JSError(rt, "Expected number for radius");
    }

    if (obj.hasProperty(rt, "minZoom")) {
      jsi::Value minZoom = obj.getProperty(rt, "minZoom");
      if (minZoom.isNumber()) {
        options.minZoom = (int)minZoom.asNumber();
      } else
        throw jsi::JSError(rt, "Expected number for minZoom");
    }

    if (obj.hasProperty(rt, "maxZoom")) {
      jsi::Value maxZoom = obj.getProperty(rt, "maxZoom");
      if (maxZoom.isNumber()) {
        options.maxZoom = (int)maxZoom.asNumber();
      } else
        throw jsi::JSError(rt, "Expected number for maxZoom");
    }

    if (obj.hasProperty(rt, "extent")) {
      jsi::Value extent = obj.getProperty(rt, "extent");
      if (extent.isNumber()) {
        options.extent = (int)extent.asNumber();
      } else
        throw jsi::JSError(rt, "Expected number for extent");
    }
    if (obj.hasProperty(rt, "minPoints")) {
      jsi::Value minPoints = obj.getProperty(rt, "minPoints");
      if (minPoints.isNumber()) {
        options.minPoints = (int)minPoints.asNumber();
      } else
        throw jsi::JSError(rt, "Expected number for minPoints");
    }
    if (obj.hasProperty(rt, "generateId")) {
      jsi::Value generateId = obj.getProperty(rt, "generateId");
      if (generateId.isBool()) {
        options.generateId = generateId.getBool();
      } else
        throw jsi::JSError(rt, "Expected boolean for generateId");
    }
  } else
    throw jsi::JSError(rt, "Expected object for options");
};

void parseJSIFeature(jsi::Runtime &rt,
                     mapbox::feature::feature<double> &feature,
                     jsi::Value const &jsiFeature) {
  if (!jsiFeature.isObject())
    throw jsi::JSError(rt, "Expected GeoJSON Feature object");

  jsi::Object obj = jsiFeature.asObject(rt);

  // obj.type
  if (!obj.hasProperty(rt, "type") ||
      !strcmp(obj.getProperty(rt, "type").asString(rt).utf8(rt).c_str(),
              "Point"))
    throw jsi::JSError(
        rt, "Expected GeoJSON Feature object with type 'Point'");

  // obj.geometry
  if (!obj.hasProperty(rt, "geometry"))
    throw jsi::JSError(rt, "Expected geometry object");

  jsi::Value geometry = obj.getProperty(rt, "geometry");

  if (!geometry.isObject())
    throw jsi::JSError(rt, "Expected geometry object");

  jsi::Object geoObj = geometry.asObject(rt);

  // obj.geometry.coordinates
  if (!geoObj.hasProperty(rt, "coordinates"))
    throw jsi::JSError(rt, "Expected coordinates property");

  jsi::Value coordinates = geoObj.getProperty(rt, "coordinates");

  if (!coordinates.asObject(rt).isArray(rt))
    throw jsi::JSError(rt, "Expected array for coordinates");

  jsi::Array arr = coordinates.asObject(rt).asArray(rt);

  if (arr.size(rt) != 2)
    throw jsi::JSError(rt, "Expected array of size 2 for coordinates");

  jsi::Value x = arr.getValueAtIndex(rt, 0);
  jsi::Value y = arr.getValueAtIndex(rt, 1);

  if (!x.isNumber() || !y.isNumber())
    throw jsi::JSError(rt, "Expected number for coordinates");

  double lng = x.asNumber();
  double lat = y.asNumber();
  mapbox::geometry::point<double> point(lng, lat);
  feature.geometry = point;

  // obj.properties
  if (obj.hasProperty(rt, "properties") &&
      obj.getProperty(rt, "properties").isObject()) {
    jsi::Object properties = obj.getProperty(rt, "properties").asObject(rt);
    jsi::Array names = properties.getPropertyNames(rt);

    // obj.properties.{property}
    for (int i = 0; i < names.size(rt); i++) {
      jsi::String name = names.getValueAtIndex(rt, i).asString(rt);
      jsi::Value property = properties.getProperty(rt, name);

      // null
      if (property.isNull())
        feature.properties[name.utf8(rt)] = std::string("null");
      // string
      else if (property.isString())
        feature.properties[name.utf8(rt)] =
            std::string(property.asString(rt).utf8(rt));
      // bool
      else if (property.isBool())
        feature.properties[name.utf8(rt)] = (property.getBool());
      else if (property.isNumber()) {
        auto numberVal = (double)property.asNumber();
        if (numberVal == (int)numberVal)
          // int
          feature.properties[name.utf8(rt)] = std::uint64_t(numberVal);
        else
          // double
          feature.properties[name.utf8(rt)] = numberVal;
      }
    }
  }
};

void clusterToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
                  mapbox::feature::feature<double> &f) {
  //  .type
  jsiObject.setProperty(rt, "type", jsi::String::createFromUtf8(rt, "Feature"));

  // .id
  const auto itr = f.properties.find("cluster_id");
  if (itr != f.properties.end() && itr->second.is<uint64_t>()) {
    jsiObject.setProperty(
        rt, "id", jsi::Value((int)f.properties["cluster_id"].get<uint64_t>()));
  }

  // .properties
  jsi::Object properties = jsi::Object(rt);
  propertiesToJSI(rt, properties, f);
  jsiObject.setProperty(rt, "properties", properties);

  // .geometry - differs from tile geometry
  jsi::Object geometry = jsi::Object(rt);
  jsi::Array coordinates = jsi::Array(rt, 2);
  auto gem = f.geometry.get<mapbox::geometry::point<double>>();
  coordinates.setValueAtIndex(rt, 0, jsi::Value(gem.x));
  coordinates.setValueAtIndex(rt, 1, jsi::Value(gem.y));
  geometry.setProperty(rt, "type", jsi::String::createFromUtf8(rt, "Point"));
  geometry.setProperty(rt, "coordinates", coordinates);
  jsiObject.setProperty(rt, "geometry", geometry);
}

void featureToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
                  mapbox::feature::feature<std::int16_t> &f) {
  // .type
  jsiObject.setProperty(rt, "type", 1);

  // .geometry
  jsi::Array geometryContainer = jsi::Array(rt, 1);
  jsi::Array geometry = jsi::Array(rt, 2);
  auto gem = f.geometry.get<mapbox::geometry::point<std::int16_t>>();
  geometry.setValueAtIndex(rt, 0, jsi::Value((int)gem.x));
  geometry.setValueAtIndex(rt, 1, jsi::Value((int)gem.y));
  geometryContainer.setValueAtIndex(rt, 0, geometry);
  jsiObject.setProperty(rt, "geometry", geometryContainer);

  // .tags
  jsi::Object tags = jsi::Object(rt);
  propertiesToJSI(rt, tags, f);
  jsiObject.setProperty(rt, "tags", tags);

  // .id
  const auto itr = f.properties.find("cluster_id");
  if (itr != f.properties.end() && itr->second.is<uint64_t>()) {
    jsiObject.setProperty(
        rt, "id", jsi::Value((int)f.properties["cluster_id"].get<uint64_t>()));
  }
}

void propertiesToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
                     mapbox::feature::feature<double> &f) {
  for (auto &itr : f.properties) {
    auto name = jsi::String::createFromUtf8(rt, itr.first);
    auto type = itr.second.which();
    if (type == 1) {
      // Boolean
      jsiObject.setProperty(rt, name, jsi::Value(itr.second.get<bool>() == 1));
    } else if (type == 2) {
      // Integer
      jsiObject.setProperty(rt, name,
                            jsi::Value((int)itr.second.get<std::uint64_t>()));
    } else if (type == 3) {
      // Double
      jsiObject.setProperty(rt, name, jsi::Value(itr.second.get<double>()));
    } else if (type == 4) {
      // Double
      jsiObject.setProperty(rt, name, jsi::Value(itr.second.get<double>()));
    } else if (type == 5) {
      auto value = itr.second.get<std::string>();
      // null
      if ("null" == value)
        jsiObject.setProperty(rt, name, jsi::Value(nullptr));
      else
        // String
        jsiObject.setProperty(
            rt, name,
            jsi::String::createFromUtf8(rt, itr.second.get<std::string>()));
    }
  }
}

// exactly same as propertiesToJSI with &f as double
void propertiesToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
                     mapbox::feature::feature<std::int16_t> &f) {
  for (auto &itr : f.properties) {
    auto name = jsi::String::createFromUtf8(rt, itr.first);
    auto type = itr.second.which();
    if (type == 1) {
      // Boolean
      jsiObject.setProperty(rt, name, jsi::Value(itr.second.get<bool>() == 1));
    } else if (type == 2) {
      // Integer
      jsiObject.setProperty(rt, name,
                            jsi::Value((int)itr.second.get<std::uint64_t>()));
    } else if (type == 3) {
      // Double
      jsiObject.setProperty(rt, name, jsi::Value(itr.second.get<double>()));
    } else if (type == 4) {
      // Double
      jsiObject.setProperty(rt, name, jsi::Value(itr.second.get<double>()));
    } else if (type == 5) {
      auto value = itr.second.get<std::string>();
      // null
      if ("null" == value)
        jsiObject.setProperty(rt, name, jsi::Value(nullptr));
      else
        // String
        jsiObject.setProperty(
            rt, name,
            jsi::String::createFromUtf8(rt, itr.second.get<std::string>()));
    }
  }
}
} // namespace clusterer

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
  const auto itr = f.properties.find("cluster_id");
  if(itr != f.properties.end() && itr->second.is<uint64_t>()) {
    jsiObject.setProperty(
        rt, "id", jsi::Value((int)f.properties["cluster_id"].get<uint64_t>()));
  }

  //  .type
  jsiObject.setProperty(rt, "type", jsi::String::createFromUtf8(rt, "Feature"));

  // .properties
  jsi::Object properties = jsi::Object(rt);
  int origFeatureIndex = -1;
  for(auto &itr : f.properties) {
    propertiesToJSI(rt, properties, itr, origFeatureIndex);
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

  // .geometry - differs from tile geometry
  jsi::Object geometry = jsi::Object(rt);
  jsi::Array coordinates = jsi::Array(rt, 2);
  auto geo = f.geometry.get<mapbox::geometry::point<double>>();
  coordinates.setValueAtIndex(rt, 0, jsi::Value(geo.x));
  coordinates.setValueAtIndex(rt, 1, jsi::Value(geo.y));
  geometry.setProperty(rt, "type", jsi::String::createFromUtf8(rt, "Point"));
  geometry.setProperty(rt, "coordinates", coordinates);
  jsiObject.setProperty(rt, "geometry", geometry);
}

void tileToJSI(jsi::Runtime &rt, jsi::Object &jsiObject,
                  mapbox::feature::feature<std::int16_t> &f,
                  jsi::Array &featuresInput) {
  // .id
  const auto itr = f.properties.find("cluster_id");
  if(itr != f.properties.end() && itr->second.is<uint64_t>()) {
    jsiObject.setProperty(
        rt, "id", jsi::Value((int)f.properties["cluster_id"].get<uint64_t>()));
  }

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
  int origFeatureIndex = -1;
  for(auto &itr : f.properties) {
    propertiesToJSI(rt, tags, itr, origFeatureIndex);
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

void propertiesToJSI(
    jsi::Runtime &rt, jsi::Object &jsiPropertyObject,
    std::pair<const std::string, mapbox::feature::value> &itr,
    int &origFeatureIndex) {
  auto name = jsi::String::createFromUtf8(rt, itr.first);
  auto type = itr.second.which();
  if(type == 1) {  // Boolean
    jsiPropertyObject.setProperty(rt, name,
                                  jsi::Value(itr.second.get<bool>() == 1));
  } else if(type == 2) {  // Integer
    int value = (int)itr.second.get<std::uint64_t>();
    if(itr.first == "_clusterer_index") {
      origFeatureIndex = value;
    }
    jsiPropertyObject.setProperty(rt, name, value);
  } else if(type == 3) {  // Double
    jsiPropertyObject.setProperty(rt, name,
                                  jsi::Value(itr.second.get<double>()));
  } else if(type == 4) {  // Double
    jsiPropertyObject.setProperty(rt, name,
                                  jsi::Value(itr.second.get<double>()));
  } else if(type == 5) {  // String
    jsiPropertyObject.setProperty(
        rt, name,
        jsi::String::createFromUtf8(rt, itr.second.get<std::string>()));
  }
}
}  // namespace clusterer

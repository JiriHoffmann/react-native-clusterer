#include <map>
#include <jsi/jsi.h>

#include "clusterer.h"
#include "supercluster.hpp"

using namespace std;
using namespace facebook;


map<string, mapbox::supercluster::Supercluster *> superclusterMap = map<string, mapbox::supercluster::Supercluster *>();

void cluster_init(jsi::Runtime &rt, jsi::Value const &nVal, jsi::Value const &fVal, jsi::Value const &oVal)
{
    auto name = nVal.asString(rt).utf8(rt);
    auto features = parseJSIFeatures(rt, fVal);
    auto options = parseJSIOptions(rt, oVal);
    auto *supercluster = new mapbox::supercluster::Supercluster(features, options);
    superclusterMap[name] = supercluster;
}

jsi::Array cluster_getTile(jsi::Runtime &rt, const string& name, int zoom, int x, int y)
{
    mapbox::supercluster::Supercluster *supercluster = superclusterMap[name];
    auto tile = supercluster->getTile(zoom, x, y);

    jsi::Array result = jsi::Array(rt, tile.size());
    int i = 0;
    for (auto &f : tile)
    {
        result.setValueAtIndex(rt, i, featureToJSIObject(rt, f, true));
        i++;
    }
    return result;
}

jsi::Array cluster_getClusters(jsi::Runtime &rt, const string& name, double bbox[4], int zoom){
    mapbox::supercluster::Supercluster *supercluster = superclusterMap[name];
    auto cluster = supercluster->getClusters(bbox, zoom);
    jsi::Array result = jsi::Array(rt, cluster.size());

    int i = 0;
    for (auto &f : cluster)
    {
        result.setValueAtIndex(rt, i, clusterFeatureToJSIObject(rt, f));
        i++;
    }
    return result;
};


jsi::Array cluster_getChildren( jsi::Runtime &rt, const string& name,int cluster_id)
{
    mapbox::supercluster::Supercluster *supercluster = superclusterMap[name];
    auto children = supercluster->getChildren(cluster_id);
    jsi::Array result = jsi::Array(rt, children.size());
    int i = 0;
    for (auto &f : children)
    {
        result.setValueAtIndex(rt, i, featureToJSIObject(rt, f, false));
        i++;
    }

    return result;
}

jsi::Array cluster_getLeaves(jsi::Runtime &rt, const string& name, int cluster_id, int limit, int offset)
{
    mapbox::supercluster::Supercluster *supercluster = superclusterMap[name];
    auto leaves = supercluster->getLeaves(cluster_id, limit, offset);

    jsi::Array result = jsi::Array(rt, leaves.size());
    int i = 0;
    for (auto &f : leaves)
    {
        result.setValueAtIndex(rt, i, featureToJSIObject(rt, f, false));
        i++;
    }

    return result;
}

int cluster_getClusterExpansionZoom(const string& name, int cluster_id)
{
    mapbox::supercluster::Supercluster *supercluster = superclusterMap[name];
    return (int)supercluster->getClusterExpansionZoom(cluster_id);
}


void cluster_destroyCluster(const string& name){
    mapbox::supercluster::Supercluster *supercluster = superclusterMap[name];
    delete supercluster;
    superclusterMap.erase(name);
}

void cluster_cleanup(){
    for (const auto& supercluster : superclusterMap) {
        delete supercluster.second;
    }
    superclusterMap.clear();
}


/*

Helper functions

*/
mapbox::feature::feature_collection<double> parseJSIFeatures(jsi::Runtime &rt, jsi::Value const &value)
{
    mapbox::feature::feature_collection<double> features;
    if (value.asObject(rt).isArray(rt))
    {
        jsi::Array arr = value.asObject(rt).asArray(rt);
        for (int i = 0; i < arr.size(rt); i++)
        {
            mapbox::feature::feature<double> feature = parseJSIFeature(rt, arr.getValueAtIndex(rt, i));
            features.push_back(feature);
        }
    }
    else
    {
        jsi::detail::throwJSError(rt, "Expected array of GeoJSON Feature objects");
    }
    return features;
};

mapbox::supercluster::Options parseJSIOptions(jsi::Runtime &rt, jsi::Value const &value)
{
    mapbox::supercluster::Options options;
    if (value.isObject())
    {
        jsi::Object obj = value.asObject(rt);

        if (obj.hasProperty(rt, "radius"))
        {
            jsi::Value radius = obj.getProperty(rt, "radius");
            if (radius.isNumber())
            {
                options.radius = (int)radius.asNumber();
            }
            else
                jsi::detail::throwJSError(rt, "Expected number for radius");
        }

        if (obj.hasProperty(rt, "minZoom"))
        {
            jsi::Value minZoom = obj.getProperty(rt, "minZoom");
            if (minZoom.isNumber())
            {
                options.minZoom = (int)minZoom.asNumber();
            }
            else
                jsi::detail::throwJSError(rt, "Expected number for minZoom");
        }

        if (obj.hasProperty(rt, "maxZoom"))
        {
            jsi::Value maxZoom = obj.getProperty(rt, "maxZoom");
            if (maxZoom.isNumber())
            {
                options.maxZoom = (int)maxZoom.asNumber();
            }
            else
                jsi::detail::throwJSError(rt, "Expected number for maxZoom");
        }

        if (obj.hasProperty(rt, "extent"))
        {
            jsi::Value extent = obj.getProperty(rt, "extent");
            if (extent.isNumber())
            {
                options.extent = (int)extent.asNumber();
            }
            else
                jsi::detail::throwJSError(rt, "Expected number for extent");
        }
        if (obj.hasProperty(rt, "minPoints"))
        {
            jsi::Value minPoints = obj.getProperty(rt, "minPoints");
            if (minPoints.isNumber())
            {
                options.minPoints = (int)minPoints.asNumber();
            }
            else
                jsi::detail::throwJSError(rt, "Expected number for minPoints");
        }
        if (obj.hasProperty(rt, "generateId"))
        {
            jsi::Value generateId = obj.getProperty(rt, "generateId");
            if (generateId.isBool())
            {
                options.generateId = generateId.getBool();
            }
            else
                jsi::detail::throwJSError(rt, "Expected boolean for generateId");
        }
    }
    else
        jsi::detail::throwJSError(rt, "Expected object for options");
    return options;
};

mapbox::feature::feature<double> parseJSIFeature(jsi::Runtime &rt, jsi::Value const &f)
{
    mapbox::feature::feature<double> feature;

    if (!f.isObject())
        jsi::detail::throwJSError(rt, "Expected GeoJSON Feature object");

    jsi::Object obj = f.asObject(rt);

    // obj.type
    if (!obj.hasProperty(rt, "type") || !strcmp(obj.getProperty(rt, "type").asString(rt).utf8(rt).c_str(), "Point"))
        jsi::detail::throwJSError(rt, "Expected GeoJSON Feature object with type 'Point'");

    // obj.geometry
    if (!obj.hasProperty(rt, "geometry"))
        jsi::detail::throwJSError(rt, "Expected geometry object");

    jsi::Value geometry = obj.getProperty(rt, "geometry");

    if (!geometry.isObject())
        jsi::detail::throwJSError(rt, "Expected geometry object");

    jsi::Object geoObj = geometry.asObject(rt);

    // obj.geometry.coordinates
    if (!geoObj.hasProperty(rt, "coordinates"))
        jsi::detail::throwJSError(rt, "Expected coordinates property");

    jsi::Value coordinates = geoObj.getProperty(rt, "coordinates");

    if (!coordinates.asObject(rt).isArray(rt))
        jsi::detail::throwJSError(rt, "Expected array for coordinates");

    jsi::Array arr = coordinates.asObject(rt).asArray(rt);

    if (arr.size(rt) != 2)
        jsi::detail::throwJSError(rt, "Expected array of size 2 for coordinates");

     jsi::Value x = arr.getValueAtIndex(rt, 0);
     jsi::Value y = arr.getValueAtIndex(rt, 1);

     if (!x.isNumber() || !y.isNumber())
         jsi::detail::throwJSError(rt, "Expected number for coordinates");

      double lng = x.asNumber();
      double lat = y.asNumber();
      mapbox::geometry::point<double> point(lng, lat);
      feature.geometry = point;

    // obj.properties
    if (obj.hasProperty(rt, "properties") && obj.getProperty(rt, "properties").isObject()){
        jsi::Object properties = obj.getProperty(rt, "properties").asObject(rt);
        jsi::Array names = properties.getPropertyNames(rt);

        // obj.properties.{property}
        for(int i = 0; i < names.size(rt); i++){
            jsi::String name = names.getValueAtIndex(rt, i).asString(rt);
            jsi::Value property = properties.getProperty(rt, name);

            // null
            if (property.isNull())
                feature.properties[name.utf8(rt)] = std::string("null");
            // string
            else if (property.isString())
                feature.properties[name.utf8(rt)] = std::string(property.asString(rt).utf8(rt));
            // bool
            else if (property.isBool())
                feature.properties[name.utf8(rt)] = (property.getBool());
            else if (property.isNumber()){
                auto numberVal = (double)property.asNumber();
                if(numberVal == (int)numberVal)
                    // int
                    feature.properties[name.utf8(rt)] = std::uint64_t(numberVal);
                else
                    // double
                    feature.properties[name.utf8(rt)] = numberVal;
            }
        }
    }

    return feature;
};

jsi::Object featureToJSIObject(jsi::Runtime &rt, mapbox::feature::feature<double> &f, bool geometryAsInt)
{
    jsi::Object result = jsi::Object(rt);
    // .type
    result.setProperty(rt, "type", 1);

    // .geometry
    jsi::Array geometryContainer = jsi::Array(rt, 1);
    jsi::Array geometry = jsi::Array(rt, 2);
    auto gem = f.geometry.get<mapbox::geometry::point<double>>();
    geometry.setValueAtIndex(rt, 0, geometryAsInt ? jsi::Value((int)gem.x) : jsi::Value(gem.x));
    geometry.setValueAtIndex(rt, 1, geometryAsInt ? jsi::Value((int)gem.y) : jsi::Value(gem.y));
    geometryContainer.setValueAtIndex(rt, 0, geometry);
    result.setProperty(rt, "geometry", geometryContainer);

    // .tags
    jsi::Object tags = propertiesToJSIObject(rt,f);
    result.setProperty(rt, "tags", tags);

    // .id
    const auto itr = f.properties.find("cluster_id");
    if (itr != f.properties.end() && itr->second.is<uint64_t>())
    {
        result.setProperty(rt, "id", jsi::Value((int)f.properties["cluster_id"].get<uint64_t>()));
    }

    return result;
}

jsi::Object clusterFeatureToJSIObject(jsi::Runtime &rt, mapbox::feature::feature<double> &f)
{
    jsi::Object result = jsi::Object(rt);
    //  .type
    result.setProperty(rt, "type", jsi::String::createFromUtf8(rt, "Feature"));

    // .id
    const auto itr = f.properties.find("cluster_id");
    if (itr != f.properties.end() && itr->second.is<uint64_t>())
    {
        result.setProperty(rt, "id", jsi::Value((int)f.properties["cluster_id"].get<uint64_t>()));
    }

    // .properties
    jsi::Object properties = propertiesToJSIObject(rt,f);
    result.setProperty(rt, "properties", properties);

    // .geometry - differs from tile geometry
    jsi::Object geometry = jsi::Object(rt);
    jsi::Array coordinates = jsi::Array(rt, 2);
    auto gem = f.geometry.get<mapbox::geometry::point<double>>();
    coordinates.setValueAtIndex(rt, 0, jsi::Value(gem.x));
    coordinates.setValueAtIndex(rt, 1,  jsi::Value(gem.y));
    geometry.setProperty(rt, "type", jsi::String::createFromUtf8(rt, "Point"));
    geometry.setProperty(rt, "coordinates", coordinates);
    result.setProperty(rt, "geometry", geometry);

    return result;
}


jsi::Object propertiesToJSIObject(jsi::Runtime &rt, mapbox::feature::feature<double> &f) {
    jsi::Object result = jsi::Object(rt);
    for(auto &itr : f.properties){
            auto name = jsi::String::createFromUtf8(rt, itr.first);
            auto type = itr.second.which();
            if(type == 1){
                // Boolean
                result.setProperty(rt, name, jsi::Value(itr.second.get<bool>() == 1));
            } else if(type == 2){
                // Integer
                result.setProperty(rt, name, jsi::Value((int)itr.second.get<std::uint64_t>()));
            } else if(type == 3){
                // Double
                result.setProperty(rt, name, jsi::Value(itr.second.get<double>()));
            } else if(type == 4){
                // Double
                result.setProperty(rt, name, jsi::Value(itr.second.get<double>()));
            } else if(type == 5){
                auto value = itr.second.get<std::string>();
                // null
                if("null" == value)
                    result.setProperty(rt, name, jsi::Value(nullptr));
                else
                // String
                result.setProperty(rt, name, jsi::String::createFromUtf8(rt,itr.second.get<std::string>()));
            }
        }
    return result;
}

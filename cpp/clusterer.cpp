#include <map>
#include <jsi/jsi.h>

#include "clusterer.h"
#include "supercluster.hpp"

using namespace std;
using namespace facebook;

map<string, mapbox::supercluster::Supercluster  *> clusterMap = map<string, mapbox::supercluster::Supercluster  *>();

void cluster_init(string name, jsi::Runtime &rt, jsi::Value const &v1, jsi::Value const &v2){
    auto options = cluster_parseJSIOptions(rt, v1);
    auto features = cluster_parseJSIFeatures(rt, v2);
    auto *cluster = new mapbox::supercluster::Supercluster(features, options);
    clusterMap[name] = cluster;
}

mapbox::supercluster::TileFeatures cluster_getTile(string name, int zoom, int x, int y){
    mapbox::supercluster::Supercluster *cluster = clusterMap[name];
    return cluster->getTile(zoom, x, y);
}

mapbox::supercluster::GeoJSONFeatures cluster_getChildren(string name, int cluster_id){
    mapbox::supercluster::Supercluster *cluster = clusterMap[name];
    return cluster->getChildren(cluster_id);
}

mapbox::supercluster::GeoJSONFeatures cluster_getLeaves(string name, int cluster_id, int limit, int offset){
    mapbox::supercluster::Supercluster *cluster = clusterMap[name];
    return cluster->getLeaves(cluster_id, limit, offset);
}

int cluster_getClusterExpansionZoom(string name, int cluster_id){
    mapbox::supercluster::Supercluster *cluster = clusterMap[name];
    return (int)cluster->getClusterExpansionZoom(cluster_id);
}

mapbox::feature::feature_collection<double> cluster_parseJSIFeatures(jsi::Runtime &rt, jsi::Value const &value){
    mapbox::feature::feature_collection<double> features;
    if(value.asObject(rt).isArray(rt)){
        jsi::Array arr = value.asObject(rt).asArray(rt);
        for(int i = 0; i < arr.size(rt); i++){
            mapbox::feature::feature<double> feature = parseJSIFeature(rt, arr.getValueAtIndex(rt, i));
            features.push_back(feature);
        }
    }
    else {
        jsi::detail::throwJSError(rt, "Expected array of GeoJSON Feature objects");
    }
    return features;
};

mapbox::supercluster::Options cluster_parseJSIOptions(jsi::Runtime &rt, jsi::Value const &value) {
    mapbox::supercluster::Options options;
    if(value.isObject()){
        jsi::Object obj = value.asObject(rt);

        if(obj.hasProperty(rt, "radius")){
            jsi::Value radius = obj.getProperty(rt, "radius");
            if(radius.isNumber()){
                options.radius = (int)radius.asNumber();
            }
            else
                jsi::detail::throwJSError(rt, "Expected number for radius");
        }
        else
            jsi::detail::throwJSError(rt, "Expected radius property");

        if(obj.hasProperty(rt, "minZoom")){
            jsi::Value minZoom = obj.getProperty(rt, "minZoom");
            if(minZoom.isNumber()){
                options.minZoom = (int)minZoom.asNumber();
            }
            else
                jsi::detail::throwJSError(rt, "Expected number for minZoom");
        }
        else
            jsi::detail::throwJSError(rt, "Expected minZoom property");

        if(obj.hasProperty(rt, "maxZoom")){
            jsi::Value maxZoom = obj.getProperty(rt, "maxZoom");
            if(maxZoom.isNumber()){
                options.maxZoom = (int)maxZoom.asNumber();
            }
            else
                jsi::detail::throwJSError(rt, "Expected number for maxZoom");
        }
        else
            jsi::detail::throwJSError(rt, "Expected maxZoom property");

        if(obj.hasProperty(rt, "extent")){
            jsi::Value extent = obj.getProperty(rt, "extent");
            if(extent.isNumber()){
                options.extent = (int)extent.asNumber();
            }
            else
                jsi::detail::throwJSError(rt, "Expected number for extent");
        }
        else
            jsi::detail::throwJSError(rt, "Expected extent property");
        if(obj.hasProperty(rt, "minPoints")){
            jsi::Value minPoints = obj.getProperty(rt, "minPoints");
            if(minPoints.isNumber()){
                options.minPoints = (int)minPoints.asNumber();
            }
            else
                jsi::detail::throwJSError(rt, "Expected number for minPoints");
        }
        else
            jsi::detail::throwJSError(rt, "Expected minPoints property");
    }
    else
        jsi::detail::throwJSError(rt, "Expected object for options");
    return options;
};

mapbox::feature::feature<double> parseJSIFeature(jsi::Runtime &rt, jsi::Value const &value){
    mapbox::feature::feature<double> feature;
    if(value.isObject()){
        jsi::Object obj = value.asObject(rt);

        if(!obj.hasProperty(rt, "type") || !strcmp(obj.getProperty(rt, "type").asString(rt).utf8(rt).c_str(), "Point"))
            jsi::detail::throwJSError(rt, "Expected GeoJSON Feature object with type 'Point'");

        if(obj.hasProperty(rt, "geometry")){
            jsi::Value geometry = obj.getProperty(rt, "geometry");
            if(geometry.isObject()){
                jsi::Object geoObj = geometry.asObject(rt);
                if(geoObj.hasProperty(rt, "coordinates")){
                    jsi::Value coordinates = geoObj.getProperty(rt, "coordinates");
                    if(coordinates.asObject(rt).isArray(rt)){
                        jsi::Array arr = coordinates.asObject(rt).asArray(rt);
                        if(arr.size(rt) == 2){
                            jsi::Value x = arr.getValueAtIndex(rt, 0);
                            jsi::Value y = arr.getValueAtIndex(rt, 1);
                            if(x.isNumber() && y.isNumber()){
                                double lng = x.asNumber();
                                double lat = y.asNumber();
                                mapbox::geometry::point<double> point(lng, lat);
                                feature.geometry = point;
                            }
                            else
                                jsi::detail::throwJSError(rt, "Expected number for coordinates");
                        }
                        else
                            jsi::detail::throwJSError(rt, "Expected array of size 2 for coordinates");
                    }
                    else
                        jsi::detail::throwJSError(rt, "Expected array for coordinates");
                }
                else
                    jsi::detail::throwJSError(rt, "Expected coordinates property");
            }
            else
                jsi::detail::throwJSError(rt, "Expected geometry object");
        }
        else
            jsi::detail::throwJSError(rt, "Expected geometry property");
    }
    else
        jsi::detail::throwJSError(rt, "Expected GeoJSON Feature object");
    return feature;
};




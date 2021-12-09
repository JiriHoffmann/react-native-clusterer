#include <jsi/jsi.h>
#include "react-native-clusterer.h"
#include "clusterer.h"

using namespace facebook;
using namespace std;

namespace example
{

	void install(jsi::Runtime &jsiRuntime)
	{

		auto init = jsi::Function::createFromHostFunction(jsiRuntime, jsi::PropNameID::forAscii(jsiRuntime, "init"), 2, [](jsi::Runtime &rt, const jsi::Value &thisVal, const jsi::Value *args, size_t count) -> jsi::Value
														  {
				if (count != 3)
				{
				  jsi::detail::throwJSError(rt, "React-Native-Clusterer: init expects 3 arguments");
				  return {};
				}

				if(!args[0].isString())
				{
					jsi::detail::throwJSError(rt, "React-Native-Clusterer: init expects a string as third argument");
					return {};
				}

				cluster_init(args[0].asString(rt).utf8(rt), rt, args[1], args[2]);
				return jsi::Value(); });

		auto getTile = jsi::Function::createFromHostFunction(jsiRuntime, jsi::PropNameID::forAscii(jsiRuntime, "getTile"), 4, [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
															 {
				if (count != 4)
				{
					jsi::detail::throwJSError(rt, "React-Native-Clusterer: getTile expects 4 arguments");
					return {};
				}

				if (!args[0].isString() || !args[1].isNumber() || !args[2].isNumber() || !args[3].isNumber())
				{
					jsi::detail::throwJSError(rt, "React-Native-Clusterer: getTile expects string and 3 numbers as arguments");
					return {};
				}

				return  cluster_getTile(args[0].asString(rt).utf8(rt), rt, (int)args[1].asNumber(), (int)args[2].asNumber(), (int)args[3].asNumber()); });

		auto getChildren = jsi::Function::createFromHostFunction(jsiRuntime, jsi::PropNameID::forAscii(jsiRuntime, "getTile"), 2, [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
																 {
				if (count != 2)
				{
					jsi::detail::throwJSError(rt, "React-Native-Clusterer: getTile expects 2 arguments");
					return {};
				}

				if (!args[0].isString() || !args[1].isNumber() )
				{
					jsi::detail::throwJSError(rt, "React-Native-Clusterer: getTile expects string and a number as arguments");
					return {};
				}

				return cluster_getChildren(args[0].asString(rt).utf8(rt),rt , (int)args[1].asNumber()); });

		auto getLeaves = jsi::Function::createFromHostFunction(jsiRuntime, jsi::PropNameID::forAscii(jsiRuntime, "getLeaves"), 4, [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
															   {
				if (count != 4)
				{
					jsi::detail::throwJSError(rt, "React-Native-Clusterer: getLeaves expects 4 arguments");
					return {};
				}
					if (!args[0].isString() || !args[1].isNumber() || !args[2].isNumber() || !args[3].isNumber())
				{
					jsi::detail::throwJSError(rt, "React-Native-Clusterer: getTile expects 3 numbers as arguments");
					return {};
				}

				return cluster_getLeaves(args[0].asString(rt).utf8(rt), rt, (int)args[1].asNumber(), (int)args[2].asNumber(), (int)args[3].asNumber()); });

		auto getClusterExpansionZoom = jsi::Function::createFromHostFunction(jsiRuntime, jsi::PropNameID::forAscii(jsiRuntime, "getLeaves"), 2, [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
																			 {
				if (count != 2)
				{
					 jsi::detail::throwJSError(rt, "React-Native-Clusterer: getClusterExpansionZoom expects 2 arguments");
					 return {};
				}
				if (!args[0].isString() || !args[1].isNumber())
				{
					 jsi::detail::throwJSError(rt, "React-Native-Clusterer: getClusterExpansionZoom expects string and a number as arguments");
					 return {};
				}

				return cluster_getClusterExpansionZoom(args[0].asString(rt).utf8(rt), (int)args[1].asNumber()); });

		jsi::Object module = jsi::Object(jsiRuntime);
		module.setProperty(jsiRuntime, "init", move(init));
		module.setProperty(jsiRuntime, "getTile", move(getTile));
		module.setProperty(jsiRuntime, "getChildren", move(getChildren));
		module.setProperty(jsiRuntime, "getLeaves", move(getLeaves));
		module.setProperty(jsiRuntime, "getClusterExpansionZoom", move(getClusterExpansionZoom));

		jsiRuntime.global().setProperty(jsiRuntime, "clustererModule", move(module));
	}

}

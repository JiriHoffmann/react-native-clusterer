#include <jsi/jsi.h>
#include "react-native-clusterer.h"

using namespace facebook;
using namespace std;

namespace example
{

	void install(jsi::Runtime &jsiRuntime)
	{

		auto multiply = jsi::Function::createFromHostFunction(jsiRuntime,
															  jsi::PropNameID::forAscii(jsiRuntime,
																						"multiply"),
															  2,
															  [](jsi::Runtime &runtime,
																 const jsi::Value &thisValue,
																 const jsi::Value *arguments,
																 size_t count) -> jsi::Value
															  {
																  int x = arguments[0].getNumber();
																  int y = arguments[1].getNumber();

																  return jsi::Value(x * y);
															  });

		jsiRuntime.global().setProperty(jsiRuntime, "multiply", move(multiply));

		//		auto multiplyWithCallback = Function::createFromHostFunction(jsiRuntime,
		//																	 PropNameID::forAscii(jsiRuntime,
		//																						  "multiplyWithCallback"),
		//																	 3,
		//																	 [](Runtime &runtime,
		//																		const Value &thisValue,
		//																		const Value *arguments,
		//																		size_t count) -> Value {
		//																		 int x = arguments[0].getNumber();
		//																		 int y = arguments[1].getNumber();
		//
		//																		 arguments[2].getObject(runtime).getFunction(runtime).call(runtime, x * y);
		//
		//																		 return Value();
		//
		//																	 });
		//
		//		jsiRuntime.global().setProperty(jsiRuntime, "multiplyWithCallback", move(multiplyWithCallback));
	}

}

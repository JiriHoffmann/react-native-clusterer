package com.reactnativeclusterer;

import android.util.Log;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.module.annotations.ReactModule;

@ReactModule(name = ClustererModule.NAME)
public class ClustererModule extends ReactContextBaseJavaModule {
    public static final String NAME = "Clusterer";

    static {
      try {
        // Used to load the 'native-lib' library on application startup.
        System.loadLibrary("cpp");
      } catch (Exception ignored) {
      }
    }

    public ClustererModule(ReactApplicationContext reactContext) {
        super(reactContext);
    }

    @Override
    @NonNull
    public String getName() {
        return NAME;
    }

    private native void nativeInstall(long jsi);

    public void installLib(JavaScriptContextHolder reactContext) {

      if (reactContext.get() != 0) {
        this.nativeInstall(
          reactContext.get()
        );
      } else {
        Log.e("SimpleJsiModule", "JSI Runtime is not available in debug mode");
      }

    }
}

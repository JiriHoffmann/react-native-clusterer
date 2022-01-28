package com.reactnativeclusterer;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;

public class ClustererModule extends ReactContextBaseJavaModule {
  public static final String NAME = "Clusterer";
  private static native void initialize(long jsiPtr, String docDir);

  public ClustererModule(ReactApplicationContext reactContext) {
    super(reactContext);
  }

  @NonNull
  @Override
  public String getName() {
    return "Clusterer";
  }

  @ReactMethod(isBlockingSynchronousMethod = true)
  public boolean install() {
    try {
      System.loadLibrary("clusterer");

      ReactApplicationContext context = getReactApplicationContext();
      initialize(
        context.getJavaScriptContextHolder().get(),
        context.getFilesDir().getAbsolutePath()
      );
      return true;
    } catch (Exception exception) {
      return false;
    }
  }
}

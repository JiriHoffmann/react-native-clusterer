package com.reactnativeclusterer;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;

class ClustererModule extends ReactContextBaseJavaModule {
    static {
        System.loadLibrary("clusterer");
    }

    private static native void initialize(long jsiPtr, String docDir);
    private static native void destruct();

    public ClustererModule(ReactApplicationContext reactContext) {
        super(reactContext);
    }

    @Override
    @NonNull
    public String getName() {
        return "Clusterer";
    }

    @Override
    public void initialize() {
        super.initialize();

        ClustererModule.initialize(
            this.getReactApplicationContext().getJavaScriptContextHolder().get(),
            this.getReactApplicationContext().getFilesDir().getAbsolutePath());
    }

    @Override
    public void onCatalystInstanceDestroy() {
        ClustererModule.destruct();
    }
}

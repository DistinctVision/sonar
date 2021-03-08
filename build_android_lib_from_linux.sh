
ANDROID_NDK=${ANDROID_NDK}  # set your path to android ndk

# set your paths to libraries
EIGEN3_DIR=/usr/include/eigen3

OpenCV_INCLUDE_DIR=$(pwd)/prebuilt_libs/android/opencv/native/jni/include
OpenCV_LIB_DIR=$(pwd)/prebuilt_libs/android/opencv/native/libs/arm64-v8a


mkdir -p build-android
cd build-android

INSTALL_DIR=$(pwd)/install

# for x64
ccmake -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
    -DEIGEN3_DIR=$EIGEN3_DIR \
    -DOpenCV_INCLUDE_DIR=$OpenCV_INCLUDE_DIR \
    -DOpenCV_LIB_DIR=$OpenCV_LIB_DIR \
    -DANDROID_NDK="$ANDROID_NDK" \
    -DANDROID_NATIVE_API_LEVEL=21 \
    -DANDROID_ABI="arm64-v8a" \
    -DCMAKE_INSTALL_PREFIX:PATH="$INSTALL_DIR" \
    ..

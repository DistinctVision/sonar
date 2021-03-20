# Sonar  
Augmented reality library for Android, Windows, Linux

### Articles in Russian
- [Основы векторной алгебры](https://habr.com/ru/post/547876/)  
- [Маркеры](https://habr.com/ru/post/547898/)


## Dependencies
[OpenCV](https://github.com/opencv/opencv) with [contrib](https://github.com/opencv/opencv_contrib) - module aruco is used.  
There are prebuilt libraries for Windows and Android, but for Linux you need to build them yourself.  
    
[Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)
It is a header only library and you need to choose directory of project.  
On Ubuntu you can type in the terminal:
```
sudo apt install libeigen3-dev
```

## Building
Use CMake for building library.  
For building library for android you can use special toolchain from Android NDK. On Ubuntu you can use script build_android_lib_from_linux.sh. But you should declare ANDROID_NDK as environment variable as path to Android NDK or replace by path it in script.  

Also you can open project sonar.pro in Qt for testing library immediate. This is special copy of project for active tests.  
Set environment variable for project such as:  
* EIGEN_DIR - path to Eigen library.
* OPENCV_INCLUDE_PATH - path to include files of OpenCV (not necessary for Windows).
* OPENCV_LIB_PATH - path to libraries of OpenCV (not necessary for Windows).
Qt project doesn't support for Android.


## Unity
There are simple project on Unity and plugin of library for it.  
If you want to update library in plugin then replace file of sonar library in folder of actual platform.

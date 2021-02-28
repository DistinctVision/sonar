TARGET = sonar
TEMPLATE = lib

CONFIG += c++17

CONFIG -= app_bundle
CONFIG -= qt

CONFIG += staticlib

DEFINES += SONAR_SET_EXPORT

android {
   LIBS += -L$$[QT_INSTALL_PLUGINS]/platforms/android
}

CONFIG(debug, debug|release) {
    #DESTDIR += $$PWD/../qbuild/debug
    OBJECTS_DIR = debug/obj
} else {
    #DESTDIR += $$PWD/../qbuild/release
    OBJECTS_DIR = release/obj
}

windows  {
    QMAKE_CXXFLAGS += /bigobj
}

linux {
    QMAKE_CXXFLAGS += -Wno-unused-function
}

INCLUDEPATH += $$PWD/src/

include (external/external_libs.pri)
include (src/sonar/sonar.pri)


#QT += qml quick widgets

CONFIG += console

CONFIG -= app_bundle
CONFIG -= qt

TARGET = tests

CONFIG += c++17
VERSION = 0.1

CONFIG(debug, debug|release) {
    MOC_DIR = debug/moc
    RCC_DIR = debug/rcc
    UI_DIR = debug/ui
    OBJECTS_DIR = debug/obj

    windows {
        PRE_TARGETDEPS += $$OUT_PWD/../lib/debug/sonar.lib
        LIBS += -L$$OUT_PWD/../lib/debug -lsonar
    } else: linux {
        PRE_TARGETDEPS += $$OUT_PWD/../lib/libsonar.a
        LIBS += -L$$OUT_PWD/../lib -lsonar
    }
} else {
    MOC_DIR = release/moc
    RCC_DIR = release/rcc
    UI_DIR = release/ui
    OBJECTS_DIR = release/obj

    windows {
        PRE_TARGETDEPS += $$OUT_PWD/../lib/release/sonar.lib
        LIBS += -L$$OUT_PWD/../lib/release -lsonar
    } else: linux {
        PRE_TARGETDEPS += $$OUT_PWD/../lib/libsonar.a
        LIBS += -L$$OUT_PWD/../lib -lsonar
    }
    LIBS += -L$$OUT_PWD/../lib -lsonar
}

linux {
    QMAKE_CXXFLAGS += -Wno-unused-function
}

include (../lib/external/external_libs.pri)
include (../lib/external/opencv.pri)

INCLUDEPATH += $$PWD/../lib/include
DEPENDPATH += $$PWD/../lib/include

INCLUDEPATH += $$PWD/../lib/src

SOURCES += \
    main.cpp \
    test_marker_transform.cpp

HEADERS += \
    test_marker_transform.h

DEFINES += _USE_MATH_DEFINES

include (General/General.pri)
include (DebugTools/DebugTools.pri)
include (ImageTools/ImageTools.pri)
include (CameraTools/CameraTools.pri)
include (ThreadsTools/ThreadsTools.pri)

HEADERS += \
    $$PWD/AbstractTrackingSystem.h \
    $$PWD/MarkerFinder.h \
    $$PWD/MarkerTrackingSystem.h \
    $$PWD/global_types.h \
    $$PWD/Sonar_c.h

SOURCES += \
    $$PWD/AbstractTrackingSystem.cpp \
    $$PWD/MarkerFinder.cpp \
    $$PWD/MarkerTrackingSystem.cpp \
    $$PWD/Sonar_c.cpp

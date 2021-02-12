include (General/General.pri)
include (DebugTools/DebugTools.pri)
include (ImageTools/ImageTools.pri)
include (CameraTools/CameraTools.pri)

HEADERS += \
    $$PWD/MarkerFinder.h \
    $$PWD/global_types.h \
    $$PWD/Sonar_c.h

SOURCES += \
    $$PWD/MarkerFinder.cpp \
    $$PWD/Sonar_c.cpp

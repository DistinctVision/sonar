HEADERS += \
    $$PWD/SyncFuture.h \
    $$PWD/Worker.h \
    $$PWD/WorkerPool.h \
    $$PWD/Semaphore.h \
    $$PWD/ReadWriteMutex.h

SOURCES += \
    $$PWD/Worker.cpp \
    $$PWD/WorkerPool.cpp \
    $$PWD/Semaphore.cpp \
    $$PWD/ReadWriteMutex.cpp

DEFINES += MODULE_THREADS_TOOLS

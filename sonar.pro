TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    lib \
    tests

tests.depends = lib



TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    posix_tai.c

HEADERS += \
    posix_tai.h

DISTFILES += \
    leap-seconds.list


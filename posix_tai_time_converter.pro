TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    paconv.c

HEADERS += \
    paconv.h

DISTFILES += \
    leap-seconds.list


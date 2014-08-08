#-------------------------------------------------
#
# Project created by QtCreator 2014-05-21T21:27:26
#
#-------------------------------------------------

QT       += core gui

TARGET = mpstop
TEMPLATE = app
CONFIG += debug

include(../shared/shared.pro)

SOURCES += main.cpp\
        mainwindow.cc \
    movie.cc \
    thumbnail.cc \
    moviescene.cc \
    timeline.cc \
    selectionmodel.cc \
    cinema.cc \
    workerthread.cc \
    defsettings.cc \
    renderer.cc \
    tocrenderer.cc \
    project.cc

HEADERS  += mainwindow.h \
    movie.h \
    thumbnail.h \
    moviescene.h \
    timeline.h \
    selectionmodel.h \
    cinema.h \
    workerthread.h \
    defsettings.h \
    renderer.h \
    tocrenderer.h \
    project.h

RESOURCES += mpstop.qrc
FORMS    += mainwindow.ui

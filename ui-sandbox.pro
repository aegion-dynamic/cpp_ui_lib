QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# Include paths for header files
INCLUDEPATH += src

# Build output directories - keep root directory clean
DESTDIR = $$PWD/build
OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.rcc
UI_DIR = $$DESTDIR/.ui

# Fix for macOS AGL framework issue - use modern OpenGL instead
macx {
    QMAKE_LFLAGS += -framework OpenGL
    LIBS += -framework OpenGL
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.

SOURCES += \
    src/graphcontainer.cpp \
    src/graphlayout.cpp \
    src/graphtype.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/tacticalsolutionview.cpp \
    src/timelinedrawingobjects.cpp \
    src/timelineview.cpp \
    src/timeselectionvisualizer.cpp \
    src/twoaxisdata.cpp \
    src/twoaxisgraph.cpp \
    src/waterfalldata.cpp \
    src/waterfallgraph.cpp \
    src/drawutils.cpp \
    src/zoompanel.cpp \
    src/bdwgraph.cpp \
    src/brwgraph.cpp \
    src/btwgraph.cpp \
    src/fdwgraph.cpp \
    src/ftwgraph.cpp \
    src/ltwgraph.cpp \
    src/rtwgraph.cpp \
    src/rtwsymboldrawing.cpp \
    src/btwsymboldrawing.cpp \
    src/simulator.cpp \
    src/interactivegraphicsitem.cpp \
    src/btwinteractiveoverlay.cpp \
    src/navtimeutils.cpp \
    src/scwwindow.cpp \
    src/scwsimulator.cpp \
    src/manoeuvreoverlay.cpp

HEADERS += \
    src/graphcontainer.h \
    src/graphlayout.h \
    src/graphtype.h \
    src/mainwindow.h \
    src/tacticalsolutionview.h \
    src/timelinedrawingobjects.h \
    src/timelineview.h \
    src/timeselectionvisualizer.h \
    src/twoaxisdata.h \
    src/twoaxisgraph.h \
    src/waterfalldata.h \
    src/waterfallgraph.h \
    src/drawutils.h \
    src/zoompanel.h\
    src/bdwgraph.h \
    src/brwgraph.h \
    src/btwgraph.h \
    src/fdwgraph.h \
    src/ftwgraph.h \
    src/ltwgraph.h \
    src/rtwgraph.h \
    src/rtwsymboldrawing.h \
    src/btwsymboldrawing.h \
    src/simulator.h \
    src/interactivegraphicsitem.h \
    src/btwinteractiveoverlay.h \
    src/navtimeutils.h  \
    src/scwwindow.h \
    src/scwsimulator.h \
    src/manoeuvreoverlay.h

FORMS += \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

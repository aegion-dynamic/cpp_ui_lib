QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    graphcontainer.cpp \
    graphlayout.cpp \
    main.cpp \
    mainwindow.cpp \
    tacticalsolutionview.cpp \
    timelineview.cpp \
    timeselectionvisualizer.cpp \
    twoaxisdata.cpp \
    twoaxisgraph.cpp \
    waterfalldata.cpp \
    waterfallgraph.cpp \
    customwaterfallgraph.cpp \
    drawutils.cpp \
    zoompanel.cpp \
    bdwgraph.cpp \
    brwgraph.cpp \
    btwgraph.cpp \
    fdwgraph.cpp \
    ftwgraph.cpp \
    ltwgraph.cpp \
    rtwgraph.cpp 

HEADERS += \
    graphcontainer.h \
    graphlayout.h \
    mainwindow.h \
    tacticalsolutionview.h \
    timelineview.h \
    timeselectionvisualizer.h \
    twoaxisdata.h \
    twoaxisgraph.h \
    waterfalldata.h \
    waterfallgraph.h \
    customwaterfallgraph.h \
    drawutils.h \
    zoompanel.h\
    bdwgraph.h \
    brwgraph.h \
    btwgraph.h \
    fdwgraph.h \
    ftwgraph.h \
    ltwgraph.h \
    rtwgraph.h 

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

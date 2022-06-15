QT       += core gui
DESTDIR = $${PWD}/../../Builds/AircraftGraphs

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


QT += printsupport

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    graph.cpp \
    inputdatadialogwindow.cpp \
    main.cpp \
    parameters.cpp \
    qcustomplot.cpp \
    stylesheets.cpp \
    widget.cpp \
    widgetSecondTab.cpp

HEADERS += \
    MonitorParam.h \
    graph.h \
    inputdatadialogwindow.h \
    matfunction.h \
    parameters.h \
    qcustomplot.h \
    stylesheets.h \
    widget.h

FORMS += \
    inputdatadialogwindow.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

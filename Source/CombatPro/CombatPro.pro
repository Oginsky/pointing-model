QT       += core gui
QT       += printsupport
DESTDIR = $${PWD}/../../Builds/CombatPro

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
    Algo/combat.cpp \
    Algo/mavevr.cpp \
    Algo/pnv.cpp \
    Algo/pointingmodel.cpp \
    QCustomPlot/qcustomplot.cpp \
    Tools/Reader.cpp \
    Tools/gfunc.cpp \
    Tools/parametersset.cpp \
    Widgets/Input/angpinputbox.cpp \
    Widgets/Input/avpinputbox.cpp \
    Widgets/Input/pnvinputbox.cpp \
    Widgets/Input/posinputbox.cpp \
    Widgets/Input/readinputbox.cpp \
    Widgets/Output/angpoutputbox.cpp \
    Widgets/Output/avpoutputbox.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Algo/combat.h \
    Algo/manevr.h \
    Algo/pnv.h \
    Algo/pointingmodel.h \
    QCustomPlot/qcustomplot.h \
    Tools/Reader.h \
    Tools/gfunc.h \
    Tools/parametersset.h \
    Widgets/Input/angpinputbox.h \
    Widgets/Input/avpinputbox.h \
    Widgets/Input/pnvinputbox.h \
    Widgets/Input/posinputbox.h \
    Widgets/Input/readinputbox.h \
    Widgets/Output/angpoutputbox.h \
    Widgets/Output/avpoutputbox.h \
    mainwindow.h

FORMS += \
    Widgets/Input/PosInputBox.ui \
    Widgets/Input/angpinputbox.ui \
    Widgets/Input/avpinputbox.ui \
    Widgets/Input/pnvinputbox.ui \
    Widgets/Input/readinputbox.ui \
    Widgets/Output/angpoutputbox.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DISTFILES +=

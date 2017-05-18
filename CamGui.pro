#-------------------------------------------------
#
# Project created by QtCreator 2017-05-18T13:10:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CamGui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    camcapture.cpp

HEADERS  += mainwindow.h \
    camcapture.h

FORMS    += mainwindow.ui

HEADERS += \
    $$PWD/common/auto_array_ptr.h \
    $$PWD/common/function_cast.h \
    $$PWD/common/minmax.h \
    $$PWD/common/STLHelper.h \
    $$PWD/DriverBase/Include/mvCustomCommandDataTypes.h \
    $$PWD/DriverBase/Include/mvDriverBaseEnums.h \
    $$PWD/DriverBase/Include/mvVersionInfo.h \
    $$PWD/mvDeviceManager/Include/mvDeviceManager.h \
    $$PWD/mvDisplay/Include/mvDisplay.h \
    $$PWD/mvDisplay/Include/mvDisplayDatatypes.h \
    $$PWD/mvDisplay/Include/mvDisplayExtensions.h \
    $$PWD/mvDisplay/Include/mvDisplayWindow.h \
    $$PWD/mvDisplay/Include/mvIMPACT_acquire_display.h \
    $$PWD/mvIMPACT_CPP/mvIMPACT_acquire.h \
    $$PWD/mvIMPACT_CPP/mvIMPACT_acquire_GenICam.h \
    $$PWD/mvIMPACT_CPP/mvIMPACT_acquire_GenICam_CustomCommands.h \
    $$PWD/mvIMPACT_CPP/mvIMPACT_acquire_GenICam_FileStream.h \
    $$PWD/mvPropHandling/Include/mvPropHandlingDatatypes.h \
    $$PWD/ThirdPartySWBindings/DirectShow/Include/DirectShowAcquire.h \
    camcapture.h \

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/
LIBS += -L$$PWD/lib/ -lmvDisplay
PRE_TARGETDEPS += $$PWD/lib/mvDisplay.lib


LIBS += -L$$PWD/lib/ -lmvDeviceManager
PRE_TARGETDEPS += $$PWD/lib/mvDeviceManager.lib

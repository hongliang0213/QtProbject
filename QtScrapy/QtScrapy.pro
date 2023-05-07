#-------------------------------------------------
#
# Project created by QtCreator 2023-04-05T09:52:36
#
#-------------------------------------------------

QT       += core gui network opengl widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtScrapy
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11


SOURCES += \
        main.cpp \
        dialog.cpp \
        videoform.cpp

HEADERS += \
        dialog.h \
        videoform.h

FORMS += \
        dialog.ui \
        videoform.ui

#添加子模块
include($$PWD/adb/adb.pri)
include($$PWD/server/server.pri)
include($$PWD/decoder/decoder.pri)
include($$PWD/common/common.pri)
include($$PWD/render/render.pri)
include($$PWD/android/android.pri)
inputcontrol
include($$PWD/inputcontrol/inputcontrol.pri)
#包含目录，
INCLUDEPATH += \
       $$PWD/adb \
       $$PWD/server \
       $$PWD/decoder \
       $$PWD/common \
       $$PWD/Thrid/ffmpeg/include \
       $$PWD/render \
       $$PWD/android \
       $$PWD/inputcontrol


LIBS +=   -L$$PWD//Thrid/ffmpeg/lib -lavcodec \
          -L$$PWD//Thrid/ffmpeg/lib -lavformat \
          -L$$PWD//Thrid/ffmpeg/lib -lavutil \
          -L$$PWD//Thrid/ffmpeg/lib -lswscale \


#INCLUDEPATH += D:/Qt5Probject/QtScrapy/QtScrapy/ffmpeglib/include
#LIBS += -LD:/Qt5Probject/QtScrapy/QtScrapy/ffmpeglib/lib\
#-lavcodec -lavformat -lavutil -lswscale


#LIBS += -L$$PWD//Thrid/ffmpeg/lib -lavformat -lavcodec -lavutil

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

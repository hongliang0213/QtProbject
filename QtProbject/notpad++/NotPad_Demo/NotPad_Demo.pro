QT       += core gui core5compat

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

qtHaveModule(printsupport): QT+=printsupport


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
#    myhighlighter.cpp \
#    mytextedit.cpp

HEADERS += \
    mainwindow.h \
#    myhighlighter.h \
#    mytextedit.h

FORMS += \
    mainwindow.ui \
    mytextedit.ui

#添加子模块
include($$PWD/MyTextEdit/MyTextEdit.pri)
include($$PWD/MyHighLighter/MyHighLighter.pri)
include($$PWD/MyCodeEdit/MyCodeEdit.pri)
#包含目录，
INCLUDEPATH += \
       $$PWD/MyTextEdit
       $$PWD/MyHighLighter
       $$PWD/MyCodeEdit

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    code.qrc \
    imgs.qrc

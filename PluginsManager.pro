QT       += core gui gui-private
QT      += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    commonFunctions.cpp \
    downloadprepare.cpp \
    downloadthread.cpp \
    main.cpp \
    mainwindow.cpp \
    networkoperate.cpp

HEADERS += \
    commonFunctions.h \
    downloadprepare.h \
    downloadthread.h \
    mainwindow.h \
    networkoperate.h

FORMS += \
    mainwindow.ui

QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO /DEBUG

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
RC_FILE += image.rc

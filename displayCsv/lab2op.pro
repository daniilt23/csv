QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    arrayiterator.cpp \
    arraylist.cpp \
    entrypoint.cpp \
    iterator.cpp \
    linkedList.cpp \
    logic.cpp \
    main.cpp \
    mainwindow.cpp \
    mystring.cpp \
    sort.cpp

HEADERS += \
    appcontext.h \
    arrayiterator.h \
    arraylist.h \
    demographicData.h \
    entrypoint.h \
    graphconsts.h \
    iterator.h \
    lineinfo.h \
    linkedList.h \
    logic.h \
    mainwindow.h \
    metrics.h \
    mystring.h \
    param.h \
    programstatus.h \
    sort.h \
    status.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

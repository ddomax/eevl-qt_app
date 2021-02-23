QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    networkbusdev.cpp \
    spi.cpp

HEADERS += \
    SPI/AD9957Parameter.h \
    SPI/WinTypes.h \
    SPI/ftd2xx.h \
    SPI/libMPSSE_spi.h \
    mainwindow.h \
    networkbusdev.h \
    spi.h

FORMS += \
    mainwindow.ui

# Environment for LibMPSSE
win32: LIBS += -L$$PWD/SPI/ -llibMPSSE

INCLUDEPATH += $$PWD/SPI
DEPENDPATH += $$PWD/SPI

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

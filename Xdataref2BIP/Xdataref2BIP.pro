# Shared library without any Qt functionality
TEMPLATE = lib
QT -= gui core

CONFIG += warn_on plugin release
CONFIG -= thread exceptions qt rtti debug

VERSION = 1.0

INCLUDEPATH += ../SDK/CHeaders/XPLM
INCLUDEPATH += ../SDK/CHeaders/Wrappers
INCLUDEPATH += ../SDK/CHeaders/Widgets
INCLUDEPATH += ../hidapi-0.6.0/hidapi
INCLUDEPATH += /usr/src/linux-headers-2.6.39-020639rc4-generic/include

# Defined to use X-Plane SDK 2.0 capabilities - no backward compatibility before 9.0
DEFINES += XPLM200

win32 {
    DEFINES += APL=0 IBM=1 LIN=0
    LIBS += -L../SDK/Libraries/Win
    LIBS += -lXPLM -lXPWidgets
    LIBS += $$quote(C:\Program Files\Microsoft SDKs\Windows\v6.0A\Lib\SetupAPI.Lib)
    SOURCES += \
          ../hidapi-0.6.0/windows/hid.cpp
    TARGET = win.xpl
}

unix:!macx {
    DEFINES += APL=0 IBM=0 LIN=1
    TARGET = lin.xpl
    # WARNING! This requires the latest version of the X-SDK !!!!
    QMAKE_CXXFLAGS += -fvisibility=hidden
    SOURCES += \
          ../hidapi-0.6.0/linux/hid.c
    LIBS += `pkg-config libudev --libs`
}

macx {
    DEFINES += APL=1 IBM=0 LIN=0
    TARGET = mac.xpl
    QMAKE_LFLAGS += -flat_namespace -undefined suppress
    SOURCES += \
          ../hidapi-0.6.0/mac/hid.c
    LIBS += -framework IOKit -framework CoreFoundation

    # Build for multiple architectures.
    # The following line is only needed to build universal on PPC architectures.
    # QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
    # The following line defines for which architectures we build.
    CONFIG += x86 ppc
}

SOURCES += \
    xdataref2BIP.cpp

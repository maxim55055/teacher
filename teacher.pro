QT += widgets
QT += core network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    components/requester/consts.cpp \
    components/requester/requester.cpp \
    config.cpp \
    journalpage.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    components/requester/consts.h \
    components/requester/requester.h \
    config.h \
    journalpage.h \
    login.h \
    mainwindow.h

FORMS += \
    journalpage.ui \
    login.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    components/requester/requester.pri

QT       += core gui xml sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    constants.cpp \
    dialogs.cpp \
    downloader.cpp \
    main.cpp \
    mainwindow.cpp \
    project.cpp \
    projecttemplate.cpp \
    settings.cpp \
    utils.cpp

HEADERS += \
    constants.h \
    dialogs.h \
    downloader.h \
    mainwindow.h \
    project.h \
    projecttemplate.h \
    settings.h \
    utils.h

FORMS += \
    dlgNewProject.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx {
        LIBS += -lX11
        QT += x11extras
    }

RESOURCES += \
    Factory.qrc

DISTFILES += \
    factory.xml \
    licenses/apache-v2.0.md \
    licenses/artistic-v2.0.md \
    licenses/bsd-2.md \
    licenses/bsd-3.md \
    licenses/epl-v1.0.md \
    licenses/gnu-agpl-v3.0.md \
    licenses/gnu-fdl-v1.3.md \
    licenses/gnu-gpl-v1.0.md \
    licenses/gnu-gpl-v2.0.md \
    licenses/gnu-gpl-v3.0.md \
    licenses/gnu-lgpl-v2.1.md \
    licenses/gnu-lgpl-v3.0.md \
    licenses/mit.md \
    licenses/mpl-v2.0.md \
    licenses/unlicense.md

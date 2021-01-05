QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../core_includes/core.cpp \
    ../core_includes/message.cpp \
    src/chatbox_client.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/widgets/chatbrowser.cpp \
    src/widgets/lineedit.cpp

HEADERS += \
    ../core_includes/core.h \
    ../core_includes/message.h \
    src/chatbox_client.h \
    src/mainwindow.h \
    src/widgets/chatbrowser.h \
    src/widgets/lineedit.h

FORMS += \
    forms/mainwindow.ui

RESOURCES += \
    resources/images.qrc \
    resources/qDarkStyleSheet/style.qrc


message("Clear your project when changing kits")

#FOR MINGW x64
*-g++{
    INCLUDEPATH += $$PWD/../dependencies\libsodium\libsodium-mingw-win64\include
    LIBS += $$PWD/../dependencies/libsodium/libsodium-mingw-win64/lib/libsodium.a
}


#FOR MSVC-2019 x64
*-msvc*{
    LIBS += -L$$PWD/../dependencies/libsodium/libsodium-msvc-2019/x64/Release/v142/dynamic/ -llibsodium
    INCLUDEPATH += $$PWD/../dependencies/libsodium/libsodium-msvc-2019/include
    DEPENDPATH += $$PWD/../dependencies/libsodium/libsodium-msvc-2019/include
    PRE_TARGETDEPS += $$PWD/../dependencies/libsodium/libsodium-msvc-2019/x64/Release/v142/dynamic/libsodium.lib
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
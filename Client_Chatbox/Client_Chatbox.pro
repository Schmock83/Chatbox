QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Server_Chatbox/src/chatbox_server.cpp \
    ../Server_Chatbox/src/databasehelper.cpp \
    ../Server_Chatbox/src/user.cpp \
    ../core_includes/core.cpp \
    src/widgets/fading_information_box.cpp \
    src/chatbox_client.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/widgets/chatbrowser.cpp \
    src/widgets/lineedit.cpp \
    src/widgets/userbutton.cpp

HEADERS += \
    ../Server_Chatbox/src/chatbox_server.h \
    ../Server_Chatbox/src/databasehelper.h \
    ../Server_Chatbox/src/user.h \
    ../core_includes/core.h \
    ../core_includes/messages/base_message.h \
    ../core_includes/messages/client_messages/client_chat_message.h \
    ../core_includes/messages/client_messages/client_message.h \
    ../core_includes/messages/client_messages/client_request_message/add_contact_request_message.h \
    ../core_includes/messages/client_messages/client_request_message/client_request_message.h \
    ../core_includes/messages/client_messages/client_request_message/login_request_message.h \
    ../core_includes/messages/client_messages/client_request_message/registration_request_message.h \
    ../core_includes/messages/client_messages/client_request_message/remove_contact_request_message.h \
    ../core_includes/messages/client_messages/client_request_message/search_user_request_message.h \
    ../core_includes/messages/client_messages/client_request_message/send_older_messages_request.h \
    ../core_includes/messages/messagewrapper.h \
    ../core_includes/messages/server_messages/server_message.h \
    ../core_includes/messages/server_messages/server_response_message/error_response_message.h \
    ../core_includes/messages/server_messages/server_response_message/login_failed_response_message.h \
    ../core_includes/messages/server_messages/server_response_message/login_succeeded_response_message.h \
    ../core_includes/messages/server_messages/server_response_message/no_older_messages_available_response_message.h \
    ../core_includes/messages/server_messages/server_response_message/registration_failed_response_message.h \
    ../core_includes/messages/server_messages/server_response_message/registration_succeeded_response_message.h \
    ../core_includes/messages/server_messages/server_response_message/search_user_response_message.h \
    ../core_includes/messages/server_messages/server_response_message/server_response_message.h \
    ../core_includes/messages/server_messages/server_update_message/server_update_message.h \
    ../core_includes/messages/server_messages/server_update_message/user_add_contact_update_message.h \
    ../core_includes/messages/server_messages/server_update_message/user_add_incoming_contact_request_update_message.h \
    ../core_includes/messages/server_messages/server_update_message/user_add_outgoing_contact_request_update_message.h \
    ../core_includes/messages/server_messages/server_update_message/user_remove_contact_update_message.h \
    ../core_includes/messages/server_messages/server_update_message/user_remove_incoming_contact_request_update_message.h \
    ../core_includes/messages/server_messages/server_update_message/user_remove_outgoing_contact_request_update_message.h \
    ../core_includes/messages/server_messages/server_update_message/user_state_changed_update_message.h \
    ../core_includes/messages/server_messages/server_update_message/user_stored_contacts_update_message.h \
    ../core_includes/messages/server_messages/server_update_message/user_stored_incoming_contact_requests_update_message.h \
    ../core_includes/messages/server_messages/server_update_message/user_stored_outgoing_contact_requests_update_message.h \
    src/widgets/fading_information_box.h \
    src/chatbox_client.h \
    src/mainwindow.h \
    src/widgets/chatbrowser.h \
    src/widgets/lineedit.h \
    src/widgets/userbutton.h

FORMS += \
    forms/fading_information_box.ui \
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

#FOR CLANG
*-clang*{
    message("Change your libsodium path (include & lib) for your libsodium installation - if not install do so with: brew install libsodium")
    INCLUDEPATH += /usr/local/Cellar/libsodium/1.0.18_1/include
    LIBS += /usr/local/Cellar/libsodium/1.0.18_1/lib/libsodium.a
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

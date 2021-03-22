#ifndef LOGIN_SUCCEEDED_RESPONSE_MESSAGE_H
#define LOGIN_SUCCEEDED_RESPONSE_MESSAGE_H

#include "server_response_message.h"
#include "../../../../Client_Chatbox/src/chatbox_client.h"

class Login_Succeeded_Response_Message : public Server_Response_Message
{
private:
    QString username;
public:
    Login_Succeeded_Response_Message(QString username) {this->username = username;}
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Response_Message::writeToStream(stream);
        stream << Server_Response_Message::Server_Response_Message_Type::Login_Succeeded_Response_Message << username;
        return stream;
    }
    static Login_Succeeded_Response_Message* readMessageFromStream(QDataStream& stream)
    {
        QString username;
        stream >> username;
        return new Login_Succeeded_Response_Message(username);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client)
    {
        //delete all old items (e.g. contacts, contact_requests, chats...)
        emit chatbox_Client->clearUI();
        emit chatbox_Client->setScene(UI::Scene::mainScene);
        emit chatbox_Client->stopWelcomePageAnimation();
        emit chatbox_Client->enableButtons();
        emit chatbox_Client->clearLoginPasswordEdit();
        emit chatbox_Client->clearLoginStatusLabel();
        chatbox_Client->setCurrentUserName(username);
        emit chatbox_Client->updateUserName(username);
    }
};

#endif // LOGIN_SUCCEEDED_RESPONSE_MESSAGE_H

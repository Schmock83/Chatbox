#ifndef LOGIN_FAILED_RESPONSE_MESSAGE_H
#define LOGIN_FAILED_RESPONSE_MESSAGE_H

#include "server_response_message.h"
#include "../../../../Client_Chatbox/src/chatbox_client.h"

class Login_Failed_Response_Message : public Server_Response_Message
{
private:
    QString responseString;
public:
    Login_Failed_Response_Message(QString responseString) {this->responseString = responseString;}
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Response_Message::writeToStream(stream);
        stream << Server_Response_Message::Server_Response_Message_Type::Login_Failed_Response_Message << responseString;
        return stream;
    }
    static Login_Failed_Response_Message* readMessageFromStream(QDataStream& stream)
    {
        QString responseString;
        stream >> responseString;
        return new Login_Failed_Response_Message(responseString);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client)
    {
        chatbox_Client->setLoginError(responseString);
        emit chatbox_Client->stopWelcomePageAnimation();
        emit chatbox_Client->clearLoginPasswordEdit();
        emit chatbox_Client->enableButtons();
    }
};

#endif // LOGIN_FAILED_RESPONSE_MESSAGE_H

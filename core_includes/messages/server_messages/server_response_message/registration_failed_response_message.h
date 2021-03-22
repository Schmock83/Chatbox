#ifndef REGISTRATION_FAILED_RESPONSE_MESSAGE_H
#define REGISTRATION_FAILED_RESPONSE_MESSAGE_H

#include "server_response_message.h"
#include "../../../../Client_Chatbox/src/chatbox_client.h"

class Registration_Failed_Response_Message : public Server_Response_Message
{
    QString responseString;
public:
    Registration_Failed_Response_Message(QString responseString) {this->responseString = responseString;}
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Response_Message::writeToStream(stream);
        stream << Server_Response_Message::Server_Response_Message_Type::Registration_Failed_Response_Message << responseString;
        return stream;
    }
    static Registration_Failed_Response_Message* readMessageFromStream(QDataStream& stream)
    {
        QString responseString;
        stream >> responseString;
        return new Registration_Failed_Response_Message(responseString);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client)
    {
        emit chatbox_Client->stopWelcomePageAnimation();
        chatbox_Client->setRegistrationError(responseString);
        emit chatbox_Client->clearRegistrationPasswordEdit();
        emit chatbox_Client->enableButtons();
    }
    virtual void print() {qDebug() << "Registration_Failed_Response_Message: " << responseString;}
};

#endif // REGISTRATION_FAILED_RESPONSE_MESSAGE_H

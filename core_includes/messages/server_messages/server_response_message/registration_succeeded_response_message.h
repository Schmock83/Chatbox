#ifndef REGISTRATION_SUCCEEDED_RESPONSE_MESSAGE_H
#define REGISTRATION_SUCCEEDED_RESPONSE_MESSAGE_H

#include "server_response_message.h"

class Registration_Succeeded_Response_Message : public Server_Response_Message
{
public:
    Registration_Succeeded_Response_Message() {}
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Response_Message::writeToStream(stream);
        stream << Server_Response_Message::Server_Response_Message_Type::Registration_Succeeded_Response_Message;
        return stream;
    }
    static Registration_Succeeded_Response_Message* readMessageFromStream(QDataStream& stream)
    {
        return new Registration_Succeeded_Response_Message();
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client)
    {
        emit chatbox_Client->stopWelcomePageAnimation();
        chatbox_Client->setRegistrationStatus("Successfully registered!");
        emit chatbox_Client->clearRegistrationPasswordEdit();
        emit chatbox_Client->enableButtons();
    }
    virtual void print() {qDebug() << "Registration_Succeeded_Response_Message";}
};

#endif // REGISTRATION_SUCCEEDED_RESPONSE_MESSAGE_H

#ifndef USER_ADD_INCOMING_CONTACT_REQUEST_UPDATE_MESSAGE_H
#define USER_ADD_INCOMING_CONTACT_REQUEST_UPDATE_MESSAGE_H

#include "server_update_message.h"
#include "../../../core.h"
#include "../../../../Client_Chatbox/src/chatbox_client.h"

class User_Add_Incoming_Contact_Request_Update_Message : public Server_Update_Message
{
private:
    QString contactName;
public:
    User_Add_Incoming_Contact_Request_Update_Message(QString contactName) {this->contactName = contactName;}
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Update_Message::writeToStream(stream);
        stream << Server_Update_Message::Server_Update_Message_Type::User_Add_Incoming_Contact_Request_Update_Message << contactName;
        return stream;
    }
    static User_Add_Incoming_Contact_Request_Update_Message* readMessageFromStream(QDataStream& stream)
    {
        QString contactName;
        stream >> contactName;
        return new User_Add_Incoming_Contact_Request_Update_Message(contactName);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client)
    {
        emit chatbox_Client->addContactRequest(contactName, ContactRequestType::addIncomingContactRequest);
    }
};

#endif // USER_ADD_INCOMING_CONTACT_REQUEST_UPDATE_MESSAGE_H

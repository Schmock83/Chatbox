#ifndef USER_STORED_OUTGOING_CONTACT_REQUESTS_UPDATE_MESSAGE_H
#define USER_STORED_OUTGOING_CONTACT_REQUESTS_UPDATE_MESSAGE_H

#include "server_update_message.h"
#include "../../../core.h"
#include "../../../../Client_Chatbox/src/chatbox_client.h"

class User_Stored_Outgoing_Contact_Requests_Update_Message : public Server_Update_Message
{
private:
    QList<QString> stored_Outgoing_Contact_Requests;
public:
    User_Stored_Outgoing_Contact_Requests_Update_Message(QList<QString> stored_Outgoing_Contact_Requests)
    {
        this->stored_Outgoing_Contact_Requests = stored_Outgoing_Contact_Requests;
    }
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Update_Message::writeToStream(stream);
        stream << Server_Update_Message::Server_Update_Message_Type::User_Stored_Outgoing_Contact_Requests_Update_Message << stored_Outgoing_Contact_Requests;
        return stream;
    }
    static User_Stored_Outgoing_Contact_Requests_Update_Message* readMessageFromStream(QDataStream& stream)
    {
        QList<QString> stored_Outgoing_Contact_Requests;
        stream >> stored_Outgoing_Contact_Requests;
        return new User_Stored_Outgoing_Contact_Requests_Update_Message(stored_Outgoing_Contact_Requests);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client)
    {
        for (const auto& stored_Outgoing_Contact_Request : stored_Outgoing_Contact_Requests)
            emit chatbox_Client->addContactRequest(stored_Outgoing_Contact_Request, ContactRequestType::addOutgoingContactRequest);
    }
    virtual void print() {qDebug() << "User_Stored_Outgoing_Contact_Requests_Update_Message: " << stored_Outgoing_Contact_Requests;}
};

#endif // USER_STORED_OUTGOING_CONTACT_REQUESTS_UPDATE_MESSAGE_H

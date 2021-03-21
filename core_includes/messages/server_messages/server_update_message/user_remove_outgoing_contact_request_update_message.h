#ifndef USER_REMOVE_OUTGOING_CONTACT_REQUEST_UPDATE_MESSAGE_H
#define USER_REMOVE_OUTGOING_CONTACT_REQUEST_UPDATE_MESSAGE_H

#include "server_update_message.h"
#include "../../../core.h"

class User_Remove_Outgoing_Contact_Request_Update_Message : public Server_Update_Message
{
private:
    QString contactName;
public:
    User_Remove_Outgoing_Contact_Request_Update_Message(QString contactName) {this->contactName = contactName;}
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Update_Message::writeToStream(stream);
        stream << Server_Update_Message::Server_Update_Message_Type::User_Remove_Outgoing_Contact_Request_Update_Message << contactName;
        return stream;
    }
    static User_Remove_Outgoing_Contact_Request_Update_Message* readMessageFromStream(QDataStream& stream)
    {
        QString contactName;
        stream >> contactName;
        return new User_Remove_Outgoing_Contact_Request_Update_Message(contactName);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client) {}
    virtual void print() {qDebug() << "User_Remove_Outgoing_Contact_Request_Update_Message: " << contactName;}
};

#endif // USER_REMOVE_OUTGOING_CONTACT_REQUEST_UPDATE_MESSAGE_H

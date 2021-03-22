#ifndef SERVER_UPDATE_MESSAGE_H
#define SERVER_UPDATE_MESSAGE_H

#include "../server_message.h"

class Server_Update_Message : public Server_Message
{
protected:
    Server_Update_Message() {}
public:
    enum Server_Update_Message_Type
    {
        User_Stored_Contacts_Update_Message=0,
        User_Stored_Incoming_Contact_Requests_Update_Message,
        User_Stored_Outgoing_Contact_Requests_Update_Message,
        User_Add_Contact_Update_Message,
        User_Remove_Contact_Update_Message,
        User_Add_Incoming_Contact_Request_Update_Message,
        User_Remove_Incoming_Contact_Request_Update_Message,
        User_Add_Outgoing_Contact_Request_Update_Message,
        User_Remove_Outgoing_Contact_Request_Update_Message,
        User_State_Changed_Update_Message
    };
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Message::writeToStream(stream);
        stream << Server_Message::Server_Message_Type::Server_Update_Message;
        return stream;
    }
};

#endif // SERVER_UPDATE_MESSAGE_H

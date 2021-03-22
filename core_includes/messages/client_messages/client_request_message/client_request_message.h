#ifndef CLIENT_REQUEST_MESSAGE_H
#define CLIENT_REQUEST_MESSAGE_H

#include "../client_message.h"

class Client_Request_Message : public Client_Message
{
protected:
    Client_Request_Message() {}
public:
    enum Client_Request_Message_Type
    {
        Login_Request_Message=0,
        Registration_Request_Message,
        Send_Older_Messages_Request,
        Add_Contact_Request_Message,
        Remove_Contact_Request_Message,
        Search_User_Request_Message
    };
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Client_Message::writeToStream(stream);
        stream << Client_Message::Client_Message_Type::Client_Request_Message;
        return stream;
    }
};

#endif // CLIENT_REQUEST_MESSAGE_H

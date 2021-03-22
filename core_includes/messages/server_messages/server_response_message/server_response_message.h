#ifndef SERVER_RESPONSE_MESSAGE_H
#define SERVER_RESPONSE_MESSAGE_H

#include "../server_message.h"

class Server_Response_Message : public Server_Message
{
protected:
    Server_Response_Message() {}
public:
    enum Server_Response_Message_Type
    {
        Login_Succeeded_Response_Message=0,
        Login_Failed_Response_Message,
        Registration_Succeeded_Response_Message,
        Registration_Failed_Response_Message,
        No_Older_Messages_Available_Response_Message,
        Search_User_Response_Message,
        Error_Response_Message
    };
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Message::writeToStream(stream);
        stream << Server_Message::Server_Message_Type::Server_Response_Message;
        return stream;
    }
};

#endif // SERVER_RESPONSE_MESSAGE_H

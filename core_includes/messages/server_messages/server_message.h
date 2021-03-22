#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

#include "../base_message.h"

class Server_Message : public Base_Message
{
public:
    enum Server_Message_Type
    {
        Server_Response_Message=0,
        Server_Update_Message
    };
    Server_Message() {}
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        stream << Base_Message_Type::Server_Message;
        return stream;
    }
};

#endif // SERVER_MESSAGE_H

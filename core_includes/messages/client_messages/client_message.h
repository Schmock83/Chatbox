#ifndef CLIENT_MESSAGE_H
#define CLIENT_MESSAGE_H

#include "../base_message.h"

class Client_Message : public Base_Message
{
public:
    enum Client_Message_Type
    {
        Client_Request_Message=0,
        Client_Chat_Message
    };
    Client_Message() {}
    Base_Message_Type getBase_Message_Type()
    {
        return Base_Message_Type::Client_Message;
    }
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        stream << Base_Message_Type::Client_Message;
        return stream;
    }
    virtual void print() {}
};

#endif // CLIENT_MESSAGE_H

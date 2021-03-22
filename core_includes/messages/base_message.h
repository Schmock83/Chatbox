#ifndef BASE_MESSAGE_H
#define BASE_MESSAGE_H

#include <QDataStream>
#include <QTcpSocket>
#include <QDebug>

class Chatbox_Client;
class Chatbox_Server;

class Base_Message
{
public:
    enum Base_Message_Type
    {
        Client_Message=0,
        Server_Message
    };

    virtual QDataStream& writeToStream(QDataStream& stream) const { return stream; }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client) {}
    virtual void handleOnServerSide(Chatbox_Server* chatbox_Server, QTcpSocket* client_Socket) {}
};

#endif // BASE_MESSAGE_H

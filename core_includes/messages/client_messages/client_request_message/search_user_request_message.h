#ifndef SEARCH_USER_REQUEST_MESSAGE_H
#define SEARCH_USER_REQUEST_MESSAGE_H

#include "client_request_message.h"

class Search_User_Request_Message : public Client_Request_Message
{
private:
    QString username;
public:
    Search_User_Request_Message(QString username) {this->username = username;}
    QDataStream& writeToStream(QDataStream& stream) const
    {
        Client_Request_Message::writeToStream(stream);
        stream << Client_Request_Message_Type::Search_User_Request_Message << username;
        return stream;
    }
    static Search_User_Request_Message* readMessageFromStream(QDataStream& stream)
    {
        QString username;
        stream >> username;
        return new Search_User_Request_Message(username);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client) {}
    void print() { qDebug() << "Search_User_Request_Message: " << username; }
};

#endif // SEARCH_USER_REQUEST_MESSAGE_H

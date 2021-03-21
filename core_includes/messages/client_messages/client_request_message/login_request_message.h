#ifndef LOGIN_REQUEST_MESSAGE_H
#define LOGIN_REQUEST_MESSAGE_H

#include "client_request_message.h"
#include "../core_includes/core.h"

class Login_Request_Message : public Client_Request_Message
{
private:
    QString username, password;
public:
    Login_Request_Message(QString username, QString password) {this->username = username; this->password = password;}
    void hashPassword()
    {
        QString hashed_password;
        hashed_password = CRYPTO::hashPassword(this->password);
        this->password = hashed_password;
    }
    QDataStream& writeToStream(QDataStream& stream) const
    {
        Client_Request_Message::writeToStream(stream);
        stream << Client_Request_Message_Type::Login_Request_Message << username << password;
        return stream;
    }
    static Login_Request_Message* readMessageFromStream(QDataStream& stream)
    {
        QString username, password;
        stream >> username >> password;
        return new Login_Request_Message(username, password);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client) {}
    void print() { Client_Request_Message::print(); qDebug() << "Login_Request_Message: " << username << " - " << password; }
};

#endif // LOGIN_REQUEST_MESSAGE_H

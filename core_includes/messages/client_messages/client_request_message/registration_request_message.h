#ifndef REGISTRATION_REQUEST_MESSAGE_H
#define REGISTRATION_REQUEST_MESSAGE_H

#include "client_request_message.h"

#include "../../../core.h"

class Registration_Request_Message : public Client_Request_Message
{
private:
    QString username, password;
public:
    Registration_Request_Message(QString username, QString password) {this->username = username; this->password = password;}
    void encryptPassword()
    {
        //hash + encrypt
        QString hashed_password = CRYPTO::hashPassword(password);
        QString encrypted_password = CRYPTO::encryptPassword(hashed_password);

        this->password = encrypted_password;
    }
    QDataStream& writeToStream(QDataStream& stream) const
    {
        Client_Request_Message::writeToStream(stream);
        stream << Client_Request_Message_Type::Registration_Request_Message << username << password;
        return stream;
    }
    static Registration_Request_Message* readMessageFromStream(QDataStream& stream)
    {
        QString username, password;
        stream >> username >> password;
        return new Registration_Request_Message(username, password);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client) {}
    void print() { qDebug() << "Registration_Request_Message: " << username << " - " << password; }
};

#endif // REGISTRATION_REQUEST_MESSAGE_H

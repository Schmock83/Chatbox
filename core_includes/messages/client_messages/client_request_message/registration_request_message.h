#ifndef REGISTRATION_REQUEST_MESSAGE_H
#define REGISTRATION_REQUEST_MESSAGE_H

#include "client_request_message.h"

#include "../../../core.h"
#include "../../../../Server_Chatbox/src/chatbox_server.h"
#include "../../server_messages/server_response_message/registration_failed_response_message.h"
#include "../../server_messages/server_response_message/registration_succeeded_response_message.h"

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
    virtual void handleOnServerSide(Chatbox_Server* chatbox_Server, QTcpSocket* client_Socket)
    {
        //if socket is already in use by another client
        if (chatbox_Server->authenticated_socket(client_Socket))
            emit client_Socket->disconnected();

        //check if username long enough
        if (username.length() < USERNAME_MIN_LEN || username.length() > USERNAME_MAX_LEN)
        {
            Base_Message* response_Message = new Registration_Failed_Response_Message(QString("The Username has to be between %1 and %2 Characters long !").arg(QString::number(USERNAME_MIN_LEN), QString::number(USERNAME_MAX_LEN)));
            chatbox_Server->queue_message(response_Message, client_Socket);
            return;
        }

        try {
            if (chatbox_Server->getDatabase()->user_registered(username)) {
                //failed - User already exists :send back message -> user with that name already exists....
                Base_Message* response_Message = new Registration_Failed_Response_Message("A User with that name already exists!");
                chatbox_Server->queue_message(response_Message, client_Socket);
                return;
            }
            chatbox_Server->getDatabase()->register_user(username, password);
            //success -> send back successfully registered ...
            Base_Message* response_Message = new Registration_Succeeded_Response_Message();
            chatbox_Server->queue_message(response_Message, client_Socket);
        }
        catch (QSqlError error) {
            qDebug() << "Error in handleRegistration: " << error.text();
            //error -> send back error on the server occured...
            Base_Message* response_Message = new Registration_Failed_Response_Message("An error occured on the server-side");
            chatbox_Server->queue_message(response_Message, client_Socket);
        }
    }
    void print() { qDebug() << "Registration_Request_Message: " << username << " - " << password; }
};

#endif // REGISTRATION_REQUEST_MESSAGE_H

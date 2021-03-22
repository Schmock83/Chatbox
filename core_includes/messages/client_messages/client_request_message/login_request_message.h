#ifndef LOGIN_REQUEST_MESSAGE_H
#define LOGIN_REQUEST_MESSAGE_H

#include "client_request_message.h"
#include "../core_includes/core.h"
#include "../../../../Server_Chatbox/src/chatbox_server.h"
#include "../../server_messages/server_response_message/login_failed_response_message.h"

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
    virtual void handleOnServerSide(Chatbox_Server* chatbox_Server, QTcpSocket* client_Socket)
    {
        //user already online -> A User with that name is currently online...
        if (chatbox_Server->userOnline(username)) {
            Base_Message* response_Message = new Login_Failed_Response_Message("A User with that name is currently online");
            chatbox_Server->queue_message(response_Message, client_Socket);
            return;
        }

        try {
            if (chatbox_Server->getDatabase()->verify_user(username, password)) {
                //user verified
                try {
                    //check if user already online
                    //user already online -> A User with that name is currently online... ||-> need to check again, because of multi-threads
                    if (chatbox_Server->userOnline(username)) {
                        Base_Message* response_Message = new Login_Failed_Response_Message("A User with that name is currently online");
                        chatbox_Server->queue_message(response_Message, client_Socket);
                        return;
                    }
                    //if socket is associated with another user -> disconnect that other user
                    else if (chatbox_Server->authenticated_socket(client_Socket)) {
                        emit client_Socket->disconnected();
                    }

                    //build a user from db
                    User* user = new User(username, chatbox_Server->getDatabase(), client_Socket);

                    chatbox_Server->user_connected(user);
                }
                catch (QSqlError error) {
                    qDebug() << "Error in handleLogin(update_last_login): " << error.text();
                }
            }
            else {
                //login failed -> Send back Password or Username wrong
                Base_Message* response_Message = new Login_Failed_Response_Message("Wrong Username or Password!");
                chatbox_Server->queue_message(response_Message, client_Socket);
                QThread::currentThread()->sleep(2);
                return;
            }
        }
        catch (QSqlError error) {
            qDebug() << "Error in handleLogin: " << error.text();
            //error -> send back error on the server occured...
            Base_Message* response_Message = new Login_Failed_Response_Message("An error occured on the server-side");
            chatbox_Server->queue_message(response_Message, client_Socket);
        }
    }
    void print() { Client_Request_Message::print(); qDebug() << "Login_Request_Message: " << username << " - " << password; }
};

#endif // LOGIN_REQUEST_MESSAGE_H

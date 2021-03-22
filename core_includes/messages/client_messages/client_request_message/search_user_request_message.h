#ifndef SEARCH_USER_REQUEST_MESSAGE_H
#define SEARCH_USER_REQUEST_MESSAGE_H

#include "client_request_message.h"
#include "../../../../Server_Chatbox/src/chatbox_server.h"
#include "../../server_messages/server_response_message/search_user_response_message.h"

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
    virtual void handleOnServerSide(Chatbox_Server* chatbox_Server, QTcpSocket* client_Socket)
    {
        //check if message came from authenticated user
        User* user = chatbox_Server->get_user_for_socket(client_Socket);
        if (user == nullptr)
        {
            client_Socket->abort();
            return;
        }

        try {
            QList<QString> found_users = chatbox_Server->getDatabase()->get_users_like(username);
            //remove the user itself
            found_users.removeAll(user->get_user_name());
            Base_Message* search_User_Result = new Search_User_Response_Message(found_users);
            chatbox_Server->queue_message(search_User_Result, user);
        }
        catch (QSqlError error) {
            //error -> send back empty list...
            Base_Message* search_User_Result = new Search_User_Response_Message(QList<QString>());
            chatbox_Server->queue_message(search_User_Result, user);
            chatbox_Server->sendErrorMessage(user);
        }
        QThread::currentThread()->sleep(1);
    }
};

#endif // SEARCH_USER_REQUEST_MESSAGE_H

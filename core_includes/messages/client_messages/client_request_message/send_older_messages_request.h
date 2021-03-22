#ifndef SEND_OLDER_MESSAGES_REQUEST_H
#define SEND_OLDER_MESSAGES_REQUEST_H

#include <QDateTime>

#include "client_request_message.h"
#include "../../../../Server_Chatbox/src/chatbox_server.h"
#include "../../server_messages/server_response_message/no_older_messages_available_response_message.h"

class Send_Older_Messages_Request : public Client_Request_Message
{
private:
    QString chatUserName;
    QDateTime dateTime;
public:
    Send_Older_Messages_Request(QString chatUserName, QDateTime dateTime)
    {
        this->chatUserName = chatUserName;
        this->dateTime = dateTime;
    }
    QDataStream& writeToStream(QDataStream& stream) const
    {
        Client_Request_Message::writeToStream(stream);
        stream << Client_Request_Message_Type::Send_Older_Messages_Request << chatUserName << dateTime;
        return stream;
    }
    static Send_Older_Messages_Request* readMessageFromStream(QDataStream& stream)
    {
        QString chatUserName;
        QDateTime dateTime;
        stream >> chatUserName >> dateTime;
        return new Send_Older_Messages_Request(chatUserName, dateTime);
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
            QThread::currentThread()->sleep(2);
            QList<Base_Message*> old_messages = user->get_last_conversation(chatUserName, dateTime.date());

            if (old_messages.isEmpty()) //no older messages
            {
                Base_Message* no_Older_Messages_Response = new No_Older_Messages_Available_Response_Message(chatUserName);
                chatbox_Server->queue_message(no_Older_Messages_Response, user);
            }
            else {
                for (auto old_message : old_messages)
                    chatbox_Server->queue_message(old_message, user);
            }
        }
        catch (QSqlError error) {
            qDebug() << "Error in handleOlderMessagesRequest: " << error.text();
            Base_Message* no_Older_Messages_Response = new No_Older_Messages_Available_Response_Message(chatUserName);
            chatbox_Server->queue_message(no_Older_Messages_Response, user);
            chatbox_Server->sendErrorMessage(user);
        }
    }
    void print() { qDebug() << "Send_Older_Messages_Request: " << chatUserName << dateTime; }
};

#endif // SEND_OLDER_MESSAGES_REQUEST_H

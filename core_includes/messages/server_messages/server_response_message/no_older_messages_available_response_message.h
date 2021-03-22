#ifndef NO_OLDER_MESSAGES_AVAILABLE_RESPONSE_MESSAGE_H
#define NO_OLDER_MESSAGES_AVAILABLE_RESPONSE_MESSAGE_H

#include "server_response_message.h"
#include "../../../../Client_Chatbox/src/chatbox_client.h"

class No_Older_Messages_Available_Response_Message : public Server_Response_Message
{
private:
    QString chatUserName;
public:
    No_Older_Messages_Available_Response_Message(QString chatUserName) {this->chatUserName = chatUserName;}
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Response_Message::writeToStream(stream);
        stream << Server_Response_Message::Server_Response_Message_Type::No_Older_Messages_Available_Response_Message << chatUserName;
        return stream;
    }
    static No_Older_Messages_Available_Response_Message* readMessageFromStream(QDataStream& stream)
    {
        QString chatUserName;
        stream >> chatUserName;
        return new No_Older_Messages_Available_Response_Message(chatUserName);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client)
    {
        emit chatbox_Client->noOlderMessagesAvailable(chatUserName);
    }
    virtual void print() {qDebug() << "No_Older_Messages_Available_Response_Message: " << chatUserName;}
};

#endif // NO_OLDER_MESSAGES_AVAILABLE_RESPONSE_MESSAGE_H

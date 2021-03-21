#ifndef SEND_OLDER_MESSAGES_REQUEST_H
#define SEND_OLDER_MESSAGES_REQUEST_H

#include <QDateTime>

#include "client_request_message.h"

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
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client) {}
    void print() { qDebug() << "Send_Older_Messages_Request: " << chatUserName << dateTime; }
};

#endif // SEND_OLDER_MESSAGES_REQUEST_H

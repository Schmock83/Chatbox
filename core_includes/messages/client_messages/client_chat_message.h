#ifndef CLIENT_CHAT_MESSAGE_H
#define CLIENT_CHAT_MESSAGE_H

#include "client_message.h"
#include "../Client_Chatbox/src/chatbox_client.h"

#include <QDateTime>
#include <QDebug>

class Client_Chat_Message : public Client_Message
{
private:
    QDateTime dateTime;
    QString receiver, sender, content;
    bool newMessage = true;
public:
    Client_Chat_Message(QString receiver, QString content, QDateTime dateTime, QString sender="", bool newMessage=true)
    {
        this->receiver = receiver;
        this->content = content;
        this->dateTime = dateTime;
        this->sender = sender;
        this->newMessage = newMessage;
    }
    QString getReceiver() const {return receiver;}
    QString getSender() const {return sender;}
    QString getContent() const {return content;}
    QDateTime getDateTime() const {return dateTime;}
    bool isNewMessage() const {return newMessage;}
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Client_Message::writeToStream(stream);
        stream << Client_Message::Client_Message_Type::Client_Chat_Message << receiver << content << dateTime << sender << newMessage;
        return stream;
    }
    static Client_Chat_Message* readMessageFromStream(QDataStream& stream)
    {
        QString receiver, sender, content;
        QDateTime dateTime;
        bool newMessage;
        stream >> receiver >> content >> dateTime >> sender >> newMessage;
        return new Client_Chat_Message(receiver, content, dateTime, sender, newMessage);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client)
    {
        emit chatbox_Client->chatMessageReceived(this, chatbox_Client->getCurrentUserName());
    }
    virtual void print() {qDebug() << "Client_Chat_Message: " << receiver << " - " << sender << " - " << content << " - " << dateTime << "- " << newMessage;}
};

#endif // CLIENT_CHAT_MESSAGE_H

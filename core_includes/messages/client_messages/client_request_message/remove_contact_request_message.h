#ifndef REMOVE_CONTACT_REQUEST_MESSAGE_H
#define REMOVE_CONTACT_REQUEST_MESSAGE_H

#include "client_request_message.h"

class Remove_Contact_Request_Message : public Client_Request_Message
{
    QString contactName;
public:
    Remove_Contact_Request_Message(QString contactName) {this->contactName = contactName;}
    QDataStream& writeToStream(QDataStream& stream) const
    {
        Client_Request_Message::writeToStream(stream);
        stream << Client_Request_Message_Type::Remove_Contact_Request_Message << contactName;
        return stream;
    }
    static Remove_Contact_Request_Message* readMessageFromStream(QDataStream& stream)
    {
        QString contactName;
        stream >> contactName;
        return new Remove_Contact_Request_Message(contactName);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client) {}
    void print() { qDebug() << "Remove_Contact_Request_Message: " << contactName; }
};

#endif // REMOVE_CONTACT_REQUEST_MESSAGE_H

#ifndef ADD_CONTACT_REQUEST_MESSAGE_H
#define ADD_CONTACT_REQUEST_MESSAGE_H

#include "client_request_message.h"

class Add_Contact_Request_Message : public Client_Request_Message
{
    QString contactName;
public:
    Add_Contact_Request_Message(QString contactName) {this->contactName = contactName;}
    QDataStream& writeToStream(QDataStream& stream) const
    {
        Client_Request_Message::writeToStream(stream);
        stream << Client_Request_Message_Type::Add_Contact_Request_Message << contactName;
        return stream;
    }
    static Add_Contact_Request_Message* readMessageFromStream(QDataStream& stream)
    {
        QString contactName;
        stream >> contactName;
        return new Add_Contact_Request_Message(contactName);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client) {}
    void print() { qDebug() << "Add_Contact_Request_Message: " << contactName; }
};

#endif // ADD_CONTACT_REQUEST_MESSAGE_H

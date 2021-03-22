#ifndef USER_STORED_CONTACTS_UPDATE_MESSAGE_H
#define USER_STORED_CONTACTS_UPDATE_MESSAGE_H

#include "server_update_message.h"
#include "../../../../Client_Chatbox/src/chatbox_client.h"

#include <QList>

class User_Stored_Contacts_Update_Message : public Server_Update_Message
{
private:
    QList<QString> stored_Contacts;
public:
    User_Stored_Contacts_Update_Message(QList<QString> stored_Contacts) {this->stored_Contacts = stored_Contacts;}
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Update_Message::writeToStream(stream);
        stream << Server_Update_Message::Server_Update_Message_Type::User_Stored_Contacts_Update_Message << stored_Contacts;
        return stream;
    }
    static User_Stored_Contacts_Update_Message* readMessageFromStream(QDataStream& stream)
    {
        QList<QString> stored_Contacts;
        stream >> stored_Contacts;
        return new User_Stored_Contacts_Update_Message(stored_Contacts);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client)
    {
        for(const auto& stored_Contact : stored_Contacts)
            emit chatbox_Client->addContactSignal(stored_Contact);
    }
    virtual void print() {qDebug() << "User_Stored_Contacts_Update_Message: " << stored_Contacts;}
};

#endif // USER_STORED_CONTACTS_UPDATE_MESSAGE_H

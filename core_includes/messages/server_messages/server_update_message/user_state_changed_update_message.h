#ifndef USER_STATE_CHANGED_UPDATE_MESSAGE_H
#define USER_STATE_CHANGED_UPDATE_MESSAGE_H

#include "server_update_message.h"
#include "../../../core.h"
#include "../../../../Client_Chatbox/src/chatbox_client.h"

#include <QPair>

class User_State_Changed_Update_Message : public Server_Update_Message
{
private:
    QString userName;
    UserState userState;
public:
    User_State_Changed_Update_Message(QString userName, UserState userState)
    {
        this->userName = userName;
        this->userState = userState;
    }
    virtual QDataStream& writeToStream(QDataStream& stream) const
    {
        Server_Update_Message::writeToStream(stream);
        stream << Server_Update_Message::Server_Update_Message_Type::User_State_Changed_Update_Message << userName << userState;
        return stream;
    }
    static User_State_Changed_Update_Message* readMessageFromStream(QDataStream& stream)
    {
        QString userName;
        UserState userState;
        stream >> userName >> userState;
        return new User_State_Changed_Update_Message(userName, userState);
    }
    virtual void handleOnClientSide(Chatbox_Client* chatbox_Client)
    {
        emit chatbox_Client->userStateChanged(QPair<QString, UserState>(userName, userState));
    }
};

#endif // USER_STATE_CHANGED_UPDATE_MESSAGE_H

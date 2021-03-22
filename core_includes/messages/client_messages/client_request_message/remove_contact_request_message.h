#ifndef REMOVE_CONTACT_REQUEST_MESSAGE_H
#define REMOVE_CONTACT_REQUEST_MESSAGE_H

#include "client_request_message.h"
#include "../../../../Server_Chatbox/src/chatbox_server.h"
#include "../../server_messages/server_update_message/user_remove_contact_update_message.h"
#include "../../server_messages/server_update_message/user_remove_incoming_contact_request_update_message.h"
#include "../../server_messages/server_update_message/user_remove_outgoing_contact_request_update_message.h"

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
    virtual void handleOnServerSide(Chatbox_Server* chatbox_Server, QTcpSocket* client_Socket)
    {
        //check if message came from authenticated user
        User* user = chatbox_Server->get_user_for_socket(client_Socket);
        if (user == nullptr)
        {
            client_Socket->abort();
            return;
        }

        //get the other user -> nullptr if offline
        User* remove_contact = chatbox_Server->get_user_for_user_name(contactName);

        //if no user was found -> nothing to do
        if (!user)
            return;

        try {
            //both users are contacts
            if (user->has_contact(contactName))
            {
                //try to remove contact in db
                user->delete_user_contact(contactName);

                //send back removeContact to both users
                Base_Message* remove_Contact_Response = new User_Remove_Contact_Update_Message(contactName);
                chatbox_Server->queue_message(remove_Contact_Response, user);

                if (remove_contact != nullptr)
                {
                    Base_Message* contact_Remove_User_Response = new User_Remove_Contact_Update_Message(user->get_user_name());
                    chatbox_Server->queue_message(contact_Remove_User_Response, remove_contact);
                }
            }
            //user is denying incoming contact-request
            else if (user->has_incoming_contact_request(contactName))
            {
                //try to remove deny contact-request in db
                user->delete_user_contact(contactName);

                //send back reply to both users
                Base_Message* remove_Incoming_Contact_Response = new User_Remove_Incoming_Contact_Request_Update_Message(contactName);
                chatbox_Server->queue_message(remove_Incoming_Contact_Response, user);

                if (remove_contact != nullptr)
                {
                    Base_Message* remove_Outgoing_Contact_Response = new User_Remove_Outgoing_Contact_Request_Update_Message(user->get_user_name());
                    chatbox_Server->queue_message(remove_Outgoing_Contact_Response, remove_contact);
                }
            }
            //user is cancelling an outgoing contact-request
            else if (user->has_outgoing_contact_request(contactName))
            {
                //try to cancel contact-request in db
                user->delete_user_contact(contactName);

                //send back reply to both users
                Base_Message* contact_Remove_Outgoing_Contact_Response = new User_Remove_Outgoing_Contact_Request_Update_Message(contactName);
                chatbox_Server->queue_message(contact_Remove_Outgoing_Contact_Response, user);

                if (remove_contact != nullptr)
                {
                    Base_Message* contact_Remove_Incoming_Contact_Response = new User_Remove_Incoming_Contact_Request_Update_Message(user->get_user_name());
                    chatbox_Server->queue_message(contact_Remove_Incoming_Contact_Response, remove_contact);
                }
            }
        }
        catch (QSqlError error) {
            qDebug() << "Error in handleRemoveContactRequest: " << error.text();
            chatbox_Server->sendErrorMessage(user);
        }
    }
};

#endif // REMOVE_CONTACT_REQUEST_MESSAGE_H

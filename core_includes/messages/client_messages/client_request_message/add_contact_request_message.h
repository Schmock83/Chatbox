#ifndef ADD_CONTACT_REQUEST_MESSAGE_H
#define ADD_CONTACT_REQUEST_MESSAGE_H

#include "client_request_message.h"
#include "../../../../Server_Chatbox/src/chatbox_server.h"
#include "../../server_messages/server_update_message/user_add_contact_update_message.h"
#include "../../server_messages/server_update_message/user_state_changed_update_message.h"
#include "../../server_messages/server_update_message/user_add_outgoing_contact_request_update_message.h"
#include "../../server_messages/server_update_message/user_add_incoming_contact_request_update_message.h"

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
    virtual void handleOnServerSide(Chatbox_Server* chatbox_Server, QTcpSocket* client_Socket)
    {
        //check if message came from authenticated user
        User* user = chatbox_Server->get_user_for_socket(client_Socket);
        if (user == nullptr)
        {
            client_Socket->abort();
            return;
        }

        //see if the contact is online
        User* added_contact = chatbox_Server->get_user_for_user_name(contactName);

        //if no user was found -> nothing to do
        if (!user)
            return;

        try {
            //user accepted incoming contact-request
            if (user->has_incoming_contact_request(contactName))
            {
                //try to add contact in db
                user->add_user_contact(contactName);

                //send back addContact message
                Base_Message* add_Contact_Response_message = new User_Add_Contact_Update_Message(contactName);
                chatbox_Server->queue_message(add_Contact_Response_message, user);

                //check if user online -> if so send him addContact message as well
                if (added_contact != nullptr)
                {
                    Base_Message* Contact_Add_User_Response_message = new User_Add_Contact_Update_Message(user->get_user_name());
                    chatbox_Server->queue_message(Contact_Add_User_Response_message, added_contact);

                    if (user->get_user_state() != UserState::offline)
                    {
                        Base_Message* user_State_Changed_Message = new User_State_Changed_Update_Message(user->get_user_name(), user->get_user_state());
                        chatbox_Server->queue_message(user_State_Changed_Message, added_contact);
                    }

                    //if the contact is online -> send userStateChanged to user
                    if (added_contact->get_user_state() != UserState::offline)
                    {
                        Base_Message* contact_State_Changed_Message = new User_State_Changed_Update_Message(added_contact->get_user_name(), added_contact->get_user_state());
                        chatbox_Server->queue_message(contact_State_Changed_Message, user);
                    }
                }
            }
            //user is sending an contact-request
            else if (!user->has_outgoing_contact_request(contactName) && !user->has_contact(contactName))
            {
                //try to add contact in db
                user->add_user_contact(contactName);

                //send back addContactRequest message
                Base_Message* add_Outgoing_Contact_Request = new User_Add_Outgoing_Contact_Request_Update_Message(contactName);
                chatbox_Server->queue_message(add_Outgoing_Contact_Request, user);

                //check if user online -> if so send him addContactRequest message as well
                if (added_contact != nullptr)
                {
                    Base_Message* contact_Add_Outgoing_Contact_Request = new User_Add_Incoming_Contact_Request_Update_Message(user->get_user_name());
                    chatbox_Server->queue_message(contact_Add_Outgoing_Contact_Request, added_contact);
                }
            }
        }
        catch (QSqlError error) {
            qDebug() << "Error in handleAddContactRequest: " << error.text();
            chatbox_Server->sendErrorMessage(user);
        }
    }
    void print() { qDebug() << "Add_Contact_Request_Message: " << contactName; }
};

#endif // ADD_CONTACT_REQUEST_MESSAGE_H

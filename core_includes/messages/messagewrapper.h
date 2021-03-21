#ifndef MESSAGEWRAPPER_H
#define MESSAGEWRAPPER_H

#include <QDataStream>
#include <QTcpSocket>

#include "base_message.h"
#include "client_messages/client_message.h"
#include "client_messages/client_chat_message.h"
#include "client_messages/client_request_message/client_request_message.h"
#include "client_messages/client_request_message/login_request_message.h"
#include "client_messages/client_request_message/registration_request_message.h"
#include "client_messages/client_request_message/send_older_messages_request.h"
#include "client_messages/client_request_message/add_contact_request_message.h"
#include "client_messages/client_request_message/remove_contact_request_message.h"
#include "client_messages/client_request_message/search_user_request_message.h"

#include "server_messages/server_message.h"
#include "server_messages/server_response_message/server_response_message.h"
#include "server_messages/server_response_message/login_succeeded_response_message.h"
#include "server_messages/server_response_message/login_failed_response_message.h"
#include "server_messages/server_response_message/registration_succeeded_response_message.h"
#include "server_messages/server_response_message/registration_failed_response_message.h"
#include "server_messages/server_response_message/no_older_messages_available_response_message.h"
#include "server_messages/server_response_message/search_user_response_message.h"
#include "server_messages/server_response_message/error_response_message.h"
#include "server_messages/server_update_message/server_update_message.h"
#include "server_messages/server_update_message/user_stored_contacts_update_message.h"
#include "server_messages/server_update_message/user_stored_incoming_contact_requests_update_message.h"
#include "server_messages/server_update_message/user_stored_outgoing_contact_requests_update_message.h"
#include "server_messages/server_update_message/user_add_contact_update_message.h"
#include "server_messages/server_update_message/user_remove_contact_update_message.h"
#include "server_messages/server_update_message/user_add_incoming_contact_request_update_message.h"
#include "server_messages/server_update_message/user_remove_incoming_contact_request_update_message.h"
#include "server_messages/server_update_message/user_add_outgoing_contact_request_update_message.h"
#include "server_messages/server_update_message/user_remove_outgoing_contact_request_update_message.h"
#include "server_messages/server_update_message/user_state_changed_update_message.h"

class MessageWrapper
{
public:
    static Base_Message* readMessageFromSocket(QTcpSocket* socket)
    {
        //check if socket readable
        if (!socket->isValid())
            emit socket->disconnected();

        QDataStream in(socket);
        return MessageWrapper::readMessageFromStream(in);
    }
    static void sendMessageThroughSocket(QTcpSocket* socket, const Base_Message* base_Message)
    {
        //check if socket writable
        if (!socket->isValid())
            emit socket->disconnected();

        QDataStream out(socket);
        MessageWrapper::writeMessageToStream(out, base_Message);
        socket->flush();
    }
    static QDataStream& writeMessageToStream(QDataStream& stream, const Base_Message* base_Message)
    {
        return base_Message->writeToStream(stream);
    }

    static Base_Message* readMessageFromStream(QDataStream& stream)
    {
        Base_Message::Base_Message_Type base_Message_Type;
        stream >> base_Message_Type;
        switch (base_Message_Type)
        {
            case Base_Message::Base_Message_Type::Client_Message:
                return readClientMessageFromStream(stream);
                break;
            case Base_Message::Base_Message_Type::Server_Message:
                return readServerMessageFromStream(stream);
                break;
        }

        return nullptr;
    }
    static Base_Message* readServerMessageFromStream(QDataStream& stream)
    {
        Server_Message::Server_Message_Type server_Message_Type;
        stream >> server_Message_Type;
        switch (server_Message_Type)
        {
            case Server_Message::Server_Message_Type::Server_Response_Message:
                return readServerResponseMessageFromStream(stream);
                break;
            case Server_Message::Server_Message_Type::Server_Update_Message:
                return readServerUpdateMessageFromStream(stream);
                break;
        }

        return nullptr;
    }
    static Base_Message* readServerUpdateMessageFromStream(QDataStream& stream)
    {
        Server_Update_Message::Server_Update_Message_Type server_Update_Message_Type;
        stream >> server_Update_Message_Type;
        switch (server_Update_Message_Type)
        {
            case Server_Update_Message::Server_Update_Message_Type::User_Stored_Contacts_Update_Message:
                return User_Stored_Contacts_Update_Message::readMessageFromStream(stream);
                break;
            case Server_Update_Message::Server_Update_Message_Type::User_Stored_Incoming_Contact_Requests_Update_Message:
                return User_Stored_Incoming_Contact_Requests_Update_Message::readMessageFromStream(stream);
                break;
            case Server_Update_Message::Server_Update_Message_Type::User_Stored_Outgoing_Contact_Requests_Update_Message:
                return User_Stored_Outgoing_Contact_Requests_Update_Message::readMessageFromStream(stream);
                break;
            case Server_Update_Message::Server_Update_Message_Type::User_Add_Contact_Update_Message:
                return User_Add_Contact_Update_Message::readMessageFromStream(stream);
                break;
            case Server_Update_Message::Server_Update_Message_Type::User_Remove_Contact_Update_Message:
                return User_Remove_Contact_Update_Message::readMessageFromStream(stream);
                break;
            case Server_Update_Message::Server_Update_Message_Type::User_Add_Incoming_Contact_Request_Update_Message:
                return User_Add_Incoming_Contact_Request_Update_Message::readMessageFromStream(stream);
                break;
            case Server_Update_Message::Server_Update_Message_Type::User_Remove_Incoming_Contact_Request_Update_Message:
                return User_Remove_Incoming_Contact_Request_Update_Message::readMessageFromStream(stream);
                break;
            case Server_Update_Message::Server_Update_Message_Type::User_Add_Outgoing_Contact_Request_Update_Message:
                return User_Add_Outgoing_Contact_Request_Update_Message::readMessageFromStream(stream);
                break;
            case Server_Update_Message::Server_Update_Message_Type::User_Remove_Outgoing_Contact_Request_Update_Message:
                return User_Remove_Outgoing_Contact_Request_Update_Message::readMessageFromStream(stream);
                break;
            case Server_Update_Message::Server_Update_Message_Type::User_State_Changed_Update_Message:
                return User_State_Changed_Update_Message::readMessageFromStream(stream);
                break;
        }

        return nullptr;
    }
    static Base_Message* readServerResponseMessageFromStream(QDataStream& stream)
    {
        Server_Response_Message::Server_Response_Message_Type server_Response_Message_Type;
        stream >> server_Response_Message_Type;
        switch (server_Response_Message_Type)
        {
            case Server_Response_Message::Server_Response_Message_Type::Login_Succeeded_Response_Message:
                return Login_Succeeded_Response_Message::readMessageFromStream(stream);
                break;
            case Server_Response_Message::Server_Response_Message_Type::Login_Failed_Response_Message:
                return Login_Failed_Response_Message::readMessageFromStream(stream);
                break;
            case Server_Response_Message::Server_Response_Message_Type::Registration_Succeeded_Response_Message:
                return Registration_Succeeded_Response_Message::readMessageFromStream(stream);
                break;
            case Server_Response_Message::Server_Response_Message_Type::Registration_Failed_Response_Message:
                return Registration_Failed_Response_Message::readMessageFromStream(stream);
                break;
            case Server_Response_Message::Server_Response_Message_Type::No_Older_Messages_Available_Response_Message:
                return No_Older_Messages_Available_Response_Message::readMessageFromStream(stream);
                break;
            case Server_Response_Message::Server_Response_Message_Type::Search_User_Response_Message:
                return Search_User_Response_Message::readMessageFromStream(stream);
                break;
            case Server_Response_Message::Server_Response_Message_Type::Error_Response_Message:
                return Error_Response_Message::readMessageFromStream(stream);
                break;
        }

        return nullptr;
    }
    static Base_Message* readClientMessageFromStream(QDataStream& stream)
    {
        Client_Message::Client_Message_Type client_Message_Type;
        stream >> client_Message_Type;
        switch (client_Message_Type)
        {
            case Client_Message::Client_Message_Type::Client_Request_Message:
                return readClientRequestMessageFromStream(stream);
                break;
            case Client_Message::Client_Message_Type::Client_Chat_Message:
                return Client_Chat_Message::readMessageFromStream(stream);
                break;
        }

        return nullptr;
    }
    static Base_Message* readClientRequestMessageFromStream(QDataStream& stream)
    {
        Client_Request_Message::Client_Request_Message_Type client_Request_Message_Type;
        stream >> client_Request_Message_Type;
        switch (client_Request_Message_Type)
        {
            case Client_Request_Message::Client_Request_Message_Type::Login_Request_Message:
                return Login_Request_Message::readMessageFromStream(stream);
                break;
            case Client_Request_Message::Client_Request_Message_Type::Registration_Request_Message:
                return Registration_Request_Message::readMessageFromStream(stream);
                break;
            case Client_Request_Message::Client_Request_Message_Type::Send_Older_Messages_Request:
                return Send_Older_Messages_Request::readMessageFromStream(stream);
                break;
            case Client_Request_Message::Client_Request_Message_Type::Add_Contact_Request_Message:
                return Add_Contact_Request_Message::readMessageFromStream(stream);
                break;
            case Client_Request_Message::Client_Request_Message_Type::Remove_Contact_Request_Message:
                return Remove_Contact_Request_Message::readMessageFromStream(stream);
                break;
            case Client_Request_Message::Client_Request_Message_Type::Search_User_Request_Message:
                return Search_User_Request_Message::readMessageFromStream(stream);
                break;
        }

        return nullptr;
    }
};

#endif // MESSAGEWRAPPER_H

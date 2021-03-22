#include "chatbox_server.h"

#include "../core_includes/messages/messagewrapper.h"

void Chatbox_Server::startServer(const QHostAddress& address, quint16 port)
{
	//start database
	try {
		database->initiate_sqlite_database();
	}
	catch (QSqlError sql_error) {
		if (sql_error.text().isEmpty())
			throw Server_Error("Could not initialize database");
		else
			throw Server_Error("Could not initialize database: " + sql_error.text());
	}

	//start tcp-server
	if (!tcp_server->listen(address, port)) {
		throw Server_Error("Could not start the tcp-server: " + tcp_server->errorString());
	}

    connect(tcp_server, SIGNAL(newConnection()), this, SLOT(new_connection()));
}

void Chatbox_Server::new_connection()
{
	QTcpSocket* new_connection_socket = tcp_server->nextPendingConnection();
	connect(new_connection_socket, SIGNAL(readyRead()), this, SLOT(new_data_in_socket()));
}

void Chatbox_Server::user_disconnected()
{
	QTcpSocket* clientSocket = (QTcpSocket*)sender();

	//find user that just disconnected (later notify friends etc.)
	User* user = get_user_for_socket(clientSocket);
	//user found -> remove from authenticated_users
	if (user != nullptr) {
		notify_contacts(user);
		QMutexLocker locker(&online_user_mutex);
		authenticated_online_users.remove(user->get_user_name());
		qDebug() << "User disconnected...: " << user->get_user_name();
	}
}

void Chatbox_Server::notify_contacts(User* user)
{
	try {
		if (user->get_user_state() != UserState::offline)
		{
			auto contacts = user->get_contacts();
			for (auto contact : contacts)
			{
				User* online_contact = get_user_for_user_name(contact);
				//if contact online
				if (online_contact != nullptr)
				{
                    Base_Message* update_Message = new User_State_Changed_Update_Message(user->get_user_name(), UserState::offline);
                    MessageWrapper::sendMessageThroughSocket(online_contact->get_tcp_socket(), update_Message);
				}
			}
		}
	}
	catch (QSqlError error)
	{
		qDebug() << "error in notify_contacts: " << error.text();
	}
}

void Chatbox_Server::user_connected(User* user)
{
	if (user == nullptr || user->get_tcp_socket() == nullptr)
		return;

	QMutexLocker locker(&online_user_mutex);
	//if user disconnected, while calculating password -> disconnect
	if (!user->connected()) {
		emit user->get_tcp_socket()->disconnected();
		return;
	}

	authenticated_online_users[user->get_user_name()] = user;

	//connecting signal-slot, for when user disconnects
	connect(user->get_tcp_socket(), SIGNAL(disconnected()), this, SLOT(user_disconnected()));

	//send back successfull login
    Base_Message* response_Message = new Login_Succeeded_Response_Message(user->get_user_name());
    queue_message(response_Message, user->get_tcp_socket());

	//send the user all stored contacts + contact-requests
	send_user_contacts(user);

	//send stored messages
	for (auto stored_message : user->get_stored_user_messages())
		queue_message(stored_message, user->get_tcp_socket());

	//send old messages (last 15)
	for (auto chat_user_name : user->get_chats())
		for (auto old_message : user->get_last_messages(15, chat_user_name))
			queue_message(old_message, user->get_tcp_socket());

	//update last_login in db
	user->update_last_login();
}

void Chatbox_Server::send_user_contacts(User* user)
{
	if (user != nullptr)
	{
		try {
			auto contacts = user->get_contacts();
            Base_Message* stored_Contact_Message = new User_Stored_Contacts_Update_Message(contacts);
            queue_message(stored_Contact_Message, user);

			//go through contacts
			for (auto contact : contacts)
			{
				//see if that contact is online
				for (User* online_contact : authenticated_online_users)
				{
					if (online_contact->get_user_name() == contact && online_contact->get_user_state() != UserState::offline)
					{
						//notify the client for userStateChanged
                        Base_Message* user_State_Update_Message1 = new User_State_Changed_Update_Message(contact, online_contact->get_user_state());
                        Base_Message* user_State_Update_Message2 = new User_State_Changed_Update_Message(user->get_user_name(), user->get_user_state());
                        queue_message(user_State_Update_Message1, user);
                        queue_message(user_State_Update_Message2, online_contact);
						break;
					}
				}
			}
            Base_Message* user_Stored_Incoming_Requests_Message = new User_Stored_Incoming_Contact_Requests_Update_Message(user->get_incoming_contact_requests());
            queue_message(user_Stored_Incoming_Requests_Message, user);


            Base_Message* user_Stored_Outgoing_Requests_Message = new User_Stored_Outgoing_Contact_Requests_Update_Message(user->get_outgoing_contact_requests());
            queue_message(user_Stored_Outgoing_Requests_Message, user);
		}
		catch (QSqlError error)
		{
			qDebug() << "error in send_user_contacts: " << error.text();
		}
	}
}

void Chatbox_Server::new_data_in_socket()
{
	QTcpSocket* clientSocket = (QTcpSocket*)sender();
    do{
        Base_Message* message = MessageWrapper::readMessageFromSocket(clientSocket);
        message->print();
        QThread* thread = QThread::create([&, message, clientSocket] { message->handleOnServerSide(this, clientSocket); });

        //connect finished with deliver_queued_messages, so that after thread the main-thread can deliver possible messages through the socket (for thread-safety)
        connect(thread, SIGNAL(finished()), this, SLOT(deliver_queued_messages()));

        thread->start();
    }while(!clientSocket->atEnd());
}

void Chatbox_Server::sendErrorMessage(User* user)
{
    Base_Message* error_Message = new Error_Response_Message("An error occurd on the server side");
    queue_message(error_Message, user);
}

bool Chatbox_Server::userOnline(const QString& user_name)
{
	QMutexLocker locker(&online_user_mutex);
	for (const User* authenticated_online_user : authenticated_online_users)
	{
		if (authenticated_online_user->get_user_name() == user_name)
			return true;
	}
	return false;
}
//stores queued messages from different threads, so that mainThread can safely send them through the socket
void Chatbox_Server::queue_message(Base_Message* message, QTcpSocket* client_socket)
{
	QMutexLocker locker(&socket_mutex);
    queued_messages.append(QPair<Base_Message*, QTcpSocket*>(message, client_socket));
}
void Chatbox_Server::queue_message(Base_Message* message, User* user)
{
	QMutexLocker locker(&socket_mutex);
    queued_messages.append(QPair<Base_Message*, QTcpSocket*>(message, user->get_tcp_socket()));
}
//send queued messages through socket
void Chatbox_Server::deliver_queued_messages()
{
	QMutexLocker locker(&socket_mutex);
    QList<QPair<Base_Message*, QTcpSocket*>>::iterator it = queued_messages.begin();
	while (it != queued_messages.end()) {
		if (it->second->isValid()) {
            qDebug() << "delivering message....";
            it->first->print();
            MessageWrapper::sendMessageThroughSocket(it->second, it->first);
		}
		it = queued_messages.erase(it);
	}
}

User* Chatbox_Server::get_user_for_socket(QTcpSocket* client_socket)
{
	if (client_socket == nullptr)
		return nullptr;

	QMutexLocker locker(&online_user_mutex);
	for (User* user : authenticated_online_users)
	{
		if (user->get_tcp_socket() == client_socket)
			return user;
	}

	//no user with that socket found
	return nullptr;
}

User* Chatbox_Server::get_user_for_user_name(const QString& user_name)
{
	QMutexLocker locker(&online_user_mutex);
	for (User* user : authenticated_online_users)
	{
		if (user->get_user_name() == user_name)
			return user;
	}
	//no user with that username found
	return nullptr;
}

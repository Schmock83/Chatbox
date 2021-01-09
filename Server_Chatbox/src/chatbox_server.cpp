#include "chatbox_server.h"

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
		QMutexLocker locker(&online_user_mutex);
		authenticated_online_users.remove(user->get_user_name());
		qDebug() << "User disconnected...: " << user->get_user_name();
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
	Message reply = Message::createServerMessage(QDateTime::currentDateTime(), ServerMessageType::server_loginSucceeded, user->get_user_name());
	queue_message(reply, user->get_tcp_socket());

	//update last_login in db
	user->update_last_login();
}

void Chatbox_Server::new_data_in_socket()
{
	QTcpSocket* clientSocket = (QTcpSocket*)sender();
	Message msg = Message::readFromSocket(clientSocket);
	QThread* thread = QThread::create([&, msg, clientSocket] { handleMessage(msg, clientSocket); });
	//connect finished with deliver_queued_messages, so that after thread the main-thread can deliver possible messages through the socket (for thread-safety)
	connect(thread, SIGNAL(finished()), this, SLOT(deliver_queued_messages()));
	thread->start();
}

void Chatbox_Server::handleMessage(const Message& message, QTcpSocket* client_socket)
{
	message.print();
	if (message.getMessageType() == MessageType::client_requestMessage) {
		switch (message.getClientRequestType()) {
		case ClientRequestType::client_registrationMessage:
			handleRegistration(message, client_socket);
			break;
		case ClientRequestType::client_loginMessage:
			handleLogin(message, client_socket);
			break;
		default:
			handleUserRequest(message, client_socket);
			break;
		}
	}
}

void Chatbox_Server::handleUserRequest(const Message& message, QTcpSocket* client_socket)
{
	//check if message came from authenticated user
	User* user = get_user_for_socket(client_socket);
	if (user == nullptr)
	{
		client_socket->abort();
		return;
	}

	switch (message.getClientRequestType())
	{
	case ClientRequestType::searchUserRequest:
		handleSearchUserRequest(message, user);
		break;
	case ClientRequestType::addUserRequest:
		handleAddContactRequest(message, user);
		break;
	}
}
//TODO
void Chatbox_Server::handleAddContactRequest(const Message& message, User* user)
{
	;
}
//TODO
void Chatbox_Server::handleRemoveContactRequest(const Message& message, User* user)
{
	;
}

void Chatbox_Server::handleSearchUserRequest(const Message& message, User* user)
{
	try {
		QList<QString> found_users = database->get_users_like(message.getContent());
		//remove the user itself
		found_users.removeAll(user->get_user_name());
		Message reply = Message::createServerMessage(QDateTime::currentDateTime(), ServerMessageType::server_searchUserResult, found_users);
		queue_message(reply, user);
	}
	catch (QSqlError error) {
		qDebug() << "Error in handleRegistration: " << error.text();
		//error -> send back empty list...
		Message error_reply = Message::createServerMessage(QDateTime::currentDateTime(), ServerMessageType::server_searchUserResult, QList<QString>());
		queue_message(error_reply, user);
	}
	QThread::currentThread()->sleep(1);
}

void Chatbox_Server::handleRegistration(const Message& message, QTcpSocket* client_socket)
{
	//if socket is already in use by another client
	if (authenticated_socket(client_socket))
		emit client_socket->disconnected();

	//check if username long enough
	if (message.getSender().length() < USERNAME_MIN_LEN || message.getSender().length() > USERNAME_MAX_LEN)
	{
		Message message = Message::createServerMessage(QDateTime::currentDateTime(), ServerMessageType::server_registrationFailed, QString("The Username has to be between %1 and %2 Characters long !").arg(QString::number(USERNAME_MIN_LEN), QString::number(USERNAME_MAX_LEN)));
		queue_message(message, client_socket);
		return;
	}

	try {
		if (database->user_registered(message.getSender())) {
			//failed - User already exists :send back message -> user with that name already exists....
			Message message = Message::createServerMessage(QDateTime::currentDateTime(), ServerMessageType::server_registrationFailed, "A User with that name already exists!");
			queue_message(message, client_socket);
			return;
		}
		database->register_user(message.getSender(), message.getContent());
		//success -> send back successfully registered ...
		Message message = Message::createServerMessage(QDateTime::currentDateTime(), ServerMessageType::server_registrationSucceeded, "");
		queue_message(message, client_socket);
	}
	catch (QSqlError error) {
		qDebug() << "Error in handleRegistration: " << error.text();
		//error -> send back error on the server occured...
		Message message = Message::createServerMessage(QDateTime::currentDateTime(), ServerMessageType::server_registrationFailed, "An error occured on the server-side");
		queue_message(message, client_socket);
	}
}
void Chatbox_Server::handleLogin(const Message& message, QTcpSocket* client_socket)
{
	//user already online -> A User with that name is currently online...
	if (userOnline(message.getSender())) {
		Message reply = Message::createServerMessage(QDateTime::currentDateTime(), ServerMessageType::server_loginFailed, "A User with that name is currently online");
		queue_message(reply, client_socket);
		return;
	}

	try {
		if (database->verify_user(message.getSender(), message.getContent())) {
			//user verified
			try {
				//check if user already online
				//user already online -> A User with that name is currently online... ||-> need to check again, because of multi-threads
				if (userOnline(message.getSender())) {
					Message reply = Message::createServerMessage(QDateTime::currentDateTime(), ServerMessageType::server_loginFailed, "A User with that name is currently online");
					queue_message(reply, client_socket);
					return;
				}
				//if socket is associated with another user -> disconnect that other user
				else if (authenticated_socket(client_socket)) {
					emit client_socket->disconnected();
				}

				//build a user from db
				User* user = new User(message.getSender(), database, client_socket);

				user_connected(user);
			}
			catch (QSqlError error) {
				qDebug() << "Error in handleLogin(update_last_login): " << error.text();
			}
		}
		else {
			//login failed -> Send back Password or Username wrong
			Message reply = Message::createServerMessage(QDateTime::currentDateTime(), ServerMessageType::server_loginFailed, "Wrong Username or Password!");
			queue_message(reply, client_socket);
			QThread::currentThread()->sleep(2);
			return;
		}
	}
	catch (QSqlError error) {
		qDebug() << "Error in handleLogin: " << error.text();
		//error -> send back error on the server occured...
		Message reply = Message::createServerMessage(QDateTime::currentDateTime(), ServerMessageType::server_loginFailed, "An error occured on the server-side");
		queue_message(reply, client_socket);
	}
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
void Chatbox_Server::queue_message(Message message, QTcpSocket* client_socket)
{
	QMutexLocker locker(&socket_mutex);
	queued_messages.append(QPair<Message, QTcpSocket*>(message, client_socket));
}
void Chatbox_Server::queue_message(Message message, User* user)
{
	QMutexLocker locker(&socket_mutex);
	queued_messages.append(QPair<Message, QTcpSocket*>(message, user->get_tcp_socket()));
}
//send queued messages through socket
void Chatbox_Server::deliver_queued_messages()
{
	QMutexLocker locker(&socket_mutex);
	QList<QPair<Message, QTcpSocket*>>::iterator it = queued_messages.begin();
	while (it != queued_messages.end()) {
		if (it->second->isValid()) {
			qDebug() << "delivering message....";
			it->first.print();
			Message::sendThroughSocket(it->second, it->first);
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
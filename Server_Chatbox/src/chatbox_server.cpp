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
	switch (message.getMessageType()) {
	case MessageType::client_registrationMessage:
		handleRegistration(message, client_socket);
		break;
	case MessageType::client_loginMessage:
		break;
	}
}

void Chatbox_Server::handleRegistration(const Message& message, QTcpSocket* client_socket)
{
	try {
		if (database->user_registered(message.getSender())) {
			//failed - User already exiss :TODO send back message -> user with that name already exists....
			Message message = Message::createServerMessage(MessageType::server_registrationFailed, "A User with that name already exists!");
			queue_message(message, client_socket);
			return;
		}
		database->register_user(message.getSender(), message.getContent());
		//success -> TODO: send back successfully registered ...
		Message message = Message::createServerMessage(MessageType::server_registrationSucceeded, "");
		queue_message(message, client_socket);
	}
	catch (QSqlError error) {
		qDebug() << "Error in handleRegistration: " << error.text();
		//error -> TODO: send back error on the server occured...
		Message message = Message::createServerMessage(MessageType::server_registrationFailed, "An error occured on the server-side");
		queue_message(message, client_socket);
	}
}
//stores queued messages from different threads, so that mainThread can safely send them through the socket
void Chatbox_Server::queue_message(Message message, QTcpSocket* client_socket)
{
	QMutexLocker locker(&mutex);
	queued_messages.append(QPair<Message, QTcpSocket*>(message, client_socket));
}
//send queued messages through socket
void Chatbox_Server::deliver_queued_messages()
{
	QMutexLocker locker(&mutex);
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
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
	thread->start();
}

void Chatbox_Server::handleMessage(const Message& message, QTcpSocket* client_socket)
{
	switch (message.getMessageType()) {
	case MessageType::registrationMessage:
		handleRegistration(message, client_socket);
		break;
	case MessageType::loginMessage:
		break;
	}
}

void Chatbox_Server::handleRegistration(const Message& message, QTcpSocket* client_socket)
{
	try {
		if (database->user_registered(message.getSender())) {
			//TODO send back message -> user with that name already exists....
			return;
		}
		database->register_user(message.getSender(), message.getContent());
		//success -> TODO: send back successfully registered ...
	}
	catch (QSqlError error) {
		qDebug() << "Error in handleRegistration: " << error.text();
	}
}
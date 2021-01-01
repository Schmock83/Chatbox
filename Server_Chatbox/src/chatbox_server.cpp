#include "chatbox_server.h"

bool Chatbox_Server::startServer(const QHostAddress& address, quint16 port)
{
	if (!tcp_server->listen(address, port)) {
		throw Server_Error(tcp_server->errorString());
	}
	else {
		connect(tcp_server, SIGNAL(newConnection()), this, SLOT(new_connection()));
		return true;
	}
}

void Chatbox_Server::new_connection()
{
	qDebug() << "new connection";
	QTcpSocket* new_connection_socket = tcp_server->nextPendingConnection();
	connect(new_connection_socket, SIGNAL(readyRead()), this, SLOT(new_data_in_socket()));
}

void Chatbox_Server::new_data_in_socket()
{
	qDebug() << "received new Data";
	QTcpSocket* clientSocket = (QTcpSocket*)sender();
	Message msg = Message::readFromSocket(clientSocket);
	msg.print();
}
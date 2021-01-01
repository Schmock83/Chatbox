#ifndef CHATBOX_SERVER_H
#define CHATBOX_SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>

#include <QHash>
#include "user.h"
#include "../core_includes/message.h"

#include <QDebug>

class Server_Error {
private:
	QString error_msg;
public:
	Server_Error(QString error_msg)
		:error_msg(error_msg)
	{}
	QString get_error_message()const { return error_msg; }
};

class Chatbox_Server : public QWidget
{
	Q_OBJECT
private:
	QTcpServer* tcp_server = nullptr;
	QHash<QString, User*> authenticated_online_users;
public:
	Chatbox_Server(QWidget* parent = nullptr)
		:QWidget(parent)
		, tcp_server(new QTcpServer())
	{}
	bool startServer(const QHostAddress& address = QHostAddress::Any, quint16 port = 0);
private slots:
	void new_data_in_socket();
	void new_connection();
};

#endif // CHATBOX_SERVER_H

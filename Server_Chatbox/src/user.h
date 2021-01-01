#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>
#include <QTcpSocket>

#include "databasehelper.h"

class User
{
private:
	int user_id;
	QString user_name;
	QDateTime registry_date;
	QDateTime last_login;
	QTcpSocket* tcp_socket;
public:
	User(int user_id, QString user_name, QDateTime registry_date, QDateTime last_login = QDateTime(), QTcpSocket* socket = nullptr)
		:user_id(user_id), user_name(user_name), registry_date(registry_date), last_login(last_login), tcp_socket(socket)
	{}
	User(int user_id, QString user_name, QDateTime registry_date, QTcpSocket* socket = nullptr, QDateTime last_login = QDateTime())
		:user_id(user_id), user_name(user_name), registry_date(registry_date), last_login(last_login), tcp_socket(socket)
	{}

	void set_tcp_socket(QTcpSocket* tcp_socket) {
		tcp_socket = tcp_socket;
	}

	int get_user_id()const { return user_id; }
	QString get_user_name()const { return user_name; }
	QDateTime get_registry_date() const { return registry_date; }
	QDateTime get_last_login()const { return last_login; }
	QTcpSocket* get_tcp_socket()const { return tcp_socket; }

	bool user_never_logged_in()const { return last_login.isValid(); }
	bool user_connected()const { return (tcp_socket != nullptr && tcp_socket->isValid()); }
	bool user_online()const { return user_connected(); }
};

#endif // USER_H

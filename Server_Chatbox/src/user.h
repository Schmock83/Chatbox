#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>
#include <QTcpSocket>

#include <QDebug>

#include "databasehelper.h"

class NoUserInDatabase {
public:
	NoUserInDatabase() {}
};

class User
{
private:
	int user_id;
	QString user_name;
	QDateTime registry_date;
	QDateTime last_login;
	QTcpSocket* tcp_socket;

	DatabaseHelper* database = nullptr;
public:
	User(int user_id, QString user_name, QDateTime registry_date, QDateTime last_login = QDateTime(), QTcpSocket* socket = nullptr)
		:user_id(user_id), user_name(user_name), registry_date(registry_date), last_login(last_login), tcp_socket(socket)
	{}
	User(int user_id, QString user_name, QDateTime registry_date, QTcpSocket* socket = nullptr, QDateTime last_login = QDateTime())
		:user_id(user_id), user_name(user_name), registry_date(registry_date), last_login(last_login), tcp_socket(socket)
	{}

	User(QString user_name, DatabaseHelper* database, QTcpSocket* socket = nullptr);

	void set_tcp_socket(QTcpSocket* tcp_socket) {
		tcp_socket = tcp_socket;
	}

	void set_database(DatabaseHelper* database) {
		database = database;
	}

	int get_user_id()const { return user_id; }
	QString get_user_name()const { return user_name; }
	QDateTime get_registry_date() const { return registry_date; }
	QDateTime get_last_login()const { return last_login; }
	QTcpSocket* get_tcp_socket()const { return tcp_socket; }

	bool user_never_logged_in()const { return !last_login.isValid(); }
	bool connected()const { return (tcp_socket != nullptr && tcp_socket->isValid() && tcp_socket->state() == QTcpSocket::ConnectedState); }
	bool online()const { return connected(); }
	bool valid_database()const { return database != nullptr && database->connected(); }

	void update_last_login();
	QList<QString> get_contacts()const;
	QList<QString> get_outgoing_contact_requests()const;
	QList<QString> get_incoming_contact_requests()const;

	void print()const { qDebug() << user_id << " - " << user_name << " - " << registry_date.toString() << " - " << last_login.toString(); }
};

#endif // USER_H

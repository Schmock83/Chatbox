#ifndef CHATBOX_SERVER_H
#define CHATBOX_SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>

#include <QHash>
#include <QThread>
#include <QMutex>
#include <QPair>
#include <QList>
#include "user.h"
#include "../core_includes/message.h"
#include "databasehelper.h"

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
	DatabaseHelper* database = nullptr;
	QList < QPair<Message, QTcpSocket* >> queued_messages;
	mutable QMutex socket_mutex;
	mutable QMutex online_user_mutex;
	QHash<QString, User*> authenticated_online_users;

	void handleMessage(const Message& message, QTcpSocket* client_socket);
	void handleRegistration(const Message& message, QTcpSocket* client_socket);
	void handleLogin(const Message& message, QTcpSocket* client_socket);
	void handleUserRequest(const Message& message, QTcpSocket* client_socket);
	void handleSearchUserRequest(const Message& message, User* user);
	void handleAddContactRequest(const Message& message, User* user);
	void handleRemoveContactRequest(const Message& message, User* user);
	void handleChatMessage(Message message, User* user);

	bool userOnline(const QString& user_name);

	void user_connected(User* user);

	void queue_message(Message message, QTcpSocket* client_socket);	//queue´s messages from threads, so that the main thread can safely send them through the socket
	void queue_message(Message message, User* user); //overload

	User* get_user_for_socket(QTcpSocket* client_socket);	//returns User pointer for the user associeted with the socket (or nullptr)
	User* get_user_for_user_name(const QString& user_name);
	bool authenticated_socket(QTcpSocket* client_socket) { return get_user_for_socket(client_socket) != nullptr; }
	void send_user_contacts(User* user);
	void notify_contacts(User* user);
public:
	Chatbox_Server(QWidget* parent = nullptr)
		:QWidget(parent)
		, tcp_server(new QTcpServer())
		, database(new DatabaseHelper())
	{}
	void startServer(const QHostAddress& address = QHostAddress::Any, quint16 port = 0);
private slots:
	void new_data_in_socket();
	void new_connection();
	void user_disconnected();
	void deliver_queued_messages();
};

#endif // CHATBOX_SERVER_H

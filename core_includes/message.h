#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDataStream>
#include <QTcpSocket>
#include <QDateTime>
#include <QList>

#include <QDebug>

#include "core.h"

enum MessageType {
	invalidMessageType = -1,
	chatMessage = 0,
	client_requestMessage,
	server_message
};

enum ServerMessageType {
	invalidServerMessageType = -1,
	server_loginSucceeded = 0,
	server_loginFailed,
	server_registrationSucceeded,
	server_registrationFailed,
	server_searchUserResult
};

enum ClientRequestType {
	invalidClientRequestType = -1,
	client_loginMessage = 0,
	client_registrationMessage,
	searchUserRequest,
	addUserRequest,
	delUserRequest
};

class Message
{
private:
	MessageType messageType = MessageType::invalidMessageType;
	ClientRequestType clientRequestType = ClientRequestType::invalidClientRequestType;
	ServerMessageType serverMessageType = ServerMessageType::invalidServerMessageType;
	QDateTime dateTime;
	QString messageContents;
	QString sender;
	QString receiver;
	QList<QString> str_list;

	Message(MessageType messageType, QDateTime dateTime, QString sender, QString content = "", QString receiver = "")
		:messageType(messageType), dateTime(dateTime), messageContents(content), sender(sender), receiver(receiver)
	{}
	Message(MessageType messageType, QDateTime dateTime, QString sender, QList<QString> str_list, QString content = "", QString receiver = "")
		:messageType(messageType), dateTime(dateTime), messageContents(content), sender(sender), receiver(receiver), str_list(str_list)
	{}
	Message(MessageType messageType, ClientRequestType clientRequestType, QDateTime dateTime, QString content = "", QString sender = "")
		:messageType(messageType), clientRequestType(clientRequestType), dateTime(dateTime), sender(sender), messageContents(content)
	{}
	Message(MessageType messageType, ServerMessageType serverMessageType, QDateTime dateTime, QString content = "")
		:messageType(messageType), serverMessageType(serverMessageType), dateTime(dateTime), messageContents(content)
	{}
	Message(MessageType messageType, ServerMessageType serverMessageType, QDateTime dateTime, QList<QString> str_list)
		:messageType(messageType), serverMessageType(serverMessageType), dateTime(dateTime), str_list(str_list)
	{}

public:
	static Message createLoginMessage(QDateTime dateTime, QString username, QString unhashed_password);
	static Message createRegistrationMessage(QDateTime dateTime, QString username, QString unhashed_password);
	static Message createServerMessage(QDateTime dateTime, ServerMessageType serverMessageType, QString content);
	static Message createServerMessage(QDateTime dateTime, ServerMessageType serverMessageType, QList<QString> str_list);
	static Message createClientRequstMessage(QDateTime dateTime, ClientRequestType requestType, QString content); //for server-requests (e.g. addContact, searchUser, changeProfile...)

	static Message readFromStream(QDataStream& stream);
	static Message readFromSocket(QTcpSocket* socket);
	static QDataStream& writeToStream(QDataStream& stream, const Message& message);
	static void sendThroughSocket(QTcpSocket* socket, const Message& message);

	MessageType getMessageType()const { return messageType; }
	ClientRequestType getClientRequestType()const { return clientRequestType; }
	ServerMessageType getServerMessageType()const { return serverMessageType; }
	QString getSender()const { return sender; }
	QString getReceiver()const { return receiver; }
	QDateTime getDateTime()const { return dateTime; }
	QString getContent()const { return messageContents; }
	QList<QString> getStringList()const { return str_list; }

	void print()const {
		qDebug() << messageType << clientRequestType << serverMessageType << dateTime << sender << messageContents << receiver << str_list;
	}
};

#endif // MESSAGE_H

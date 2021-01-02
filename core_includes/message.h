#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDataStream>
#include <QTcpSocket>
#include <QDateTime>

#include <QDebug>

#include "core.h"

enum MessageType {
	client_chatMessage = 0,
	client_loginMessage,
	client_registrationMessage,
	server_loginSucceeded,
	server_loginFailed,
	server_registrationSucceeded,
	server_registrationFailed
};

class Message
{
private:
	MessageType messageType;
	QDateTime dateTime;
	QString messageContents;
	QString sender;
	QString receiver;

	Message(MessageType messageType, QDateTime dateTime, QString sender, QString content = "", QString receiver = "")
		:messageType(messageType), dateTime(dateTime), messageContents(content), sender(sender), receiver(receiver)
	{}

public:
	static Message createLoginMessage(QDateTime dateTime, QString username, QString unhashed_password);
	static Message createRegistrationMessage(QDateTime dateTime, QString username, QString unhashed_password);
	static Message createDefaultMessage(QDateTime dateTime, QString sender, QString receiver, QString content);
	static Message createServerMessage(MessageType messageType, QString content);

	static Message readFromStream(QDataStream& stream);
	static Message readFromSocket(QTcpSocket* socket);
	static QDataStream& writeToStream(QDataStream& stream, const Message& message);
	static void sendThroughSocket(QTcpSocket* socket, const Message& message);

	MessageType getMessageType()const { return messageType; }
	QString getSender()const { return sender; }
	QString getReceiver()const { return receiver; }
	QDateTime getDateTime()const { return dateTime; }
	QString getContent()const { return messageContents; }

	void print()const {
		qDebug() << messageType << dateTime << sender << messageContents << receiver;
	}
};

#endif // MESSAGE_H

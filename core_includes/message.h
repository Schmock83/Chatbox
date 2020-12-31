#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDataStream>
#include <QTcpSocket>
#include <QDateTime>

#include <QDebug>

#include "core.h"

enum MessageType {
	chatMessage = 0,
	loginMessage,
	registrationMessage
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

	static Message readFromStream(QDataStream& stream);
	static Message readFromSocket(QTcpSocket* socket);
	static QDataStream& writeToStream(QDataStream& stream, const Message& message);
	static void sendThroughSocket(QTcpSocket* socket, const Message& message);

	/*friend QDataStream& operator<<(QDataStream& stream, const Message& message) {
		stream << message.messageType << message.dateTime << message.sender << message.receiver << message.messageContents;
		return stream;
	}
	friend QDataStream& operator>>(QDataStream& stream, Message& message) {
		stream >> message.messageType >> message.dateTime >> message.sender >> message.receiver >> message.messageContents;
		return stream;
	}*/

	MessageType getMessageType()const { return messageType; }
	QString getSender()const { return sender; }
	QString getReceiver()const { return receiver; }
	QDateTime getDateTime()const { return dateTime; }
	QString getContent()const { return messageContents; }

	void print()const {
		qDebug() << messageType << dateTime << sender << receiver << messageContents;
	}
};

#endif // MESSAGE_H

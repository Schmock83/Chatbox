#include "message.h"

Message Message::createLoginMessage(QDateTime dateTime, QString username, QString password)
{
	return Message(MessageType::loginMessage, dateTime, username, password);
}
Message Message::createRegistrationMessage(QDateTime dateTime, QString username, QString password)
{
	return Message(MessageType::registrationMessage, dateTime, username, password);
}
Message Message::createDefaultMessage(QDateTime dateTime, QString sender, QString receiver, QString content)
{
	return Message(MessageType::chatMessage, dateTime, sender, content, receiver);
}

Message Message::readFromStream(QDataStream& stream) {
	MessageType messageType;
	QDateTime dateTime;
	QString messageContents;
	QString sender;
	QString receiver;

	stream >> messageType >> dateTime >> sender >> receiver >> messageContents;

	return Message(messageType, dateTime, sender, messageContents, receiver);
}
QDataStream& Message::writeToStream(QDataStream& stream, const Message& message) {
	stream << message.messageType << message.dateTime << message.sender << message.receiver << message.messageContents;
	return stream;
}
Message Message::readFromSocket(QTcpSocket* socket) {
	//check if socket readable
	if (!socket->isValid())
		emit socket->disconnected();

	QDataStream in(socket);
	return Message::readFromStream(in);
}

void Message::sendThroughSocket(QTcpSocket* socket, const Message& message) {
	//check if socket readable
	if (!socket->isValid())
		emit socket->disconnected();

	QDataStream out(socket);
	Message::writeToStream(out, message);
	socket->flush();
}
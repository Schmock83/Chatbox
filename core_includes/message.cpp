#include "message.h"

Message Message::createLoginMessage(QDateTime dateTime, QString username, QString unhashed_password)
{
	//hash
	QString hashed_password = CRYPTO::hashPassword(unhashed_password);
	return Message(MessageType::client_loginMessage, dateTime, username, hashed_password);
}
Message Message::createRegistrationMessage(QDateTime dateTime, QString username, QString unhashed_password)
{
	//hash + encrypt
	QString hashed_password = CRYPTO::hashPassword(unhashed_password);
	QString encrypted_password = CRYPTO::encryptPassword(hashed_password);
	return Message(MessageType::client_registrationMessage, dateTime, username, encrypted_password);
}
Message Message::createServerMessage(MessageType messageType, QString content)
{
	return Message(messageType, QDateTime::currentDateTime(), "", content);
}
Message Message::createServerMessage(MessageType messageType, QList<QString> str_list)
{
	return Message(messageType, QDateTime::currentDateTime(), "", str_list);
}
Message Message::createDefaultMessage(QDateTime dateTime, QString sender, QString receiver, QString content)
{
	return Message(MessageType::client_chatMessage, dateTime, sender, content, receiver);
}
Message Message::createClientRequstMessage(RequestType requestType, QString content)
{
	return Message(MessageType::client_requestMessage, requestType, content);
}

Message Message::readFromStream(QDataStream& stream)
{
	MessageType messageType;
	RequestType requestType;
	QDateTime dateTime;
	QString messageContents;
	QString sender;
	QString receiver;
	QList<QString> str_list;

	stream >> messageType >> dateTime >> sender;

	if (messageType == MessageType::client_requestMessage) {
		stream >> requestType >> messageContents;
		return Message(messageType, requestType, messageContents);
	}
	else if (messageType == MessageType::server_searchUserResult) {
		stream >> str_list;
		return Message(messageType, dateTime, sender, str_list);
	}
	else {
		stream >> receiver >> messageContents;
		return Message(messageType, dateTime, sender, messageContents, receiver);
	}
}
QDataStream& Message::writeToStream(QDataStream& stream, const Message& message)
{
	stream << message.messageType << message.dateTime << message.sender;

	if (message.messageType == MessageType::client_requestMessage) {
		stream << message.getRequestType() << message.getContent();
	}
	else if (message.messageType == MessageType::server_searchUserResult) {
		stream << message.str_list;
	}
	else {
		stream << message.receiver << message.messageContents;
	}
	return stream;
}
Message Message::readFromSocket(QTcpSocket* socket)
{
	//check if socket readable
	if (!socket->isValid())
		emit socket->disconnected();

	QDataStream in(socket);
	return Message::readFromStream(in);
}

void Message::sendThroughSocket(QTcpSocket* socket, const Message& message)
{
	//check if socket readable
	if (!socket->isValid())
		emit socket->disconnected();

	QDataStream out(socket);
	Message::writeToStream(out, message);
	socket->flush();
}
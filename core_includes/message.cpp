#include "message.h"

Message Message::createLoginMessage(QDateTime dateTime, QString username, QString unhashed_password)
{
	//hash
	QString hashed_password = CRYPTO::hashPassword(unhashed_password);
	return Message(MessageType::client_requestMessage, ClientRequestType::client_loginMessage, dateTime, hashed_password, username);
}
Message Message::createRegistrationMessage(QDateTime dateTime, QString username, QString unhashed_password)
{
	//hash + encrypt
	QString hashed_password = CRYPTO::hashPassword(unhashed_password);
	QString encrypted_password = CRYPTO::encryptPassword(hashed_password);
	return Message(MessageType::client_requestMessage, ClientRequestType::client_registrationMessage, dateTime, encrypted_password, username);
}
Message Message::createServerMessage(QDateTime dateTime, ServerMessageType serverMessageType, QString content)
{
	return Message(MessageType::server_message, serverMessageType, dateTime, content);
}
Message Message::createServerMessage(QDateTime dateTime, ServerMessageType serverMessageType, QList<QString> str_list)
{
	return Message(MessageType::server_message, serverMessageType, dateTime, str_list);
}
Message Message::createClientRequstMessage(QDateTime dateTime, ClientRequestType requestType, QString content)
{
	return Message(MessageType::client_requestMessage, requestType, dateTime, content);
}

QDataStream& Message::writeToStream(QDataStream& stream, const Message& message)
{
	stream << message.messageType << message.dateTime << message.sender;

	if (message.messageType == MessageType::client_requestMessage) {
		stream << message.getClientRequestType() << message.getContent();
	}
	else if (message.messageType == MessageType::server_message)
	{
		stream << message.serverMessageType;
		switch (message.serverMessageType)
		{
		case ServerMessageType::server_loginFailed:
		case ServerMessageType::server_loginSucceeded:
		case ServerMessageType::server_registrationFailed:
		case ServerMessageType::server_registrationSucceeded:
			stream << message.messageContents;
			break;
		case ServerMessageType::server_searchUserResult:
			stream << message.str_list;
			break;
		}
	}
	else {
		stream << message.receiver << message.messageContents;
	}
	return stream;
}

Message Message::readFromStream(QDataStream& stream)
{
	MessageType messageType;
	ClientRequestType clientRequestType;
	ServerMessageType serverMessageType;
	QDateTime dateTime;
	QString messageContents;
	QString sender;
	QString receiver;
	QList<QString> str_list;

	stream >> messageType >> dateTime >> sender;

	if (messageType == MessageType::client_requestMessage) {
		stream >> clientRequestType >> messageContents;
		return Message(messageType, clientRequestType, dateTime, messageContents, sender);
	}
	else if (messageType == MessageType::server_message)
	{
		stream >> serverMessageType;
		switch (serverMessageType)
		{
		case ServerMessageType::server_loginFailed:
		case ServerMessageType::server_loginSucceeded:
		case ServerMessageType::server_registrationFailed:
		case ServerMessageType::server_registrationSucceeded:
			stream >> messageContents;
			return Message(messageType, serverMessageType, dateTime, messageContents);
			break;
		case ServerMessageType::server_searchUserResult:
			stream >> str_list;
			return Message(messageType, serverMessageType, dateTime, str_list);
			break;
		}
	}
	else {
		stream >> receiver >> messageContents;
		return Message(messageType, dateTime, sender, messageContents, receiver);
	}
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
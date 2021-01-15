#include "message.h"
Message Message::createChatMessage(QString receiver, QDateTime dateTime, QString content, QString sender = "")
{
	return Message(MessageType::chatMessage, dateTime, sender, content, receiver);
}
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
Message Message::createServerMessage(QDateTime dateTime, ServerMessageType serverMessageType, QPair<QString, UserState> string_state_pair)
{
	return Message(MessageType::server_message, serverMessageType, dateTime, string_state_pair);
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
		case ServerMessageType::server_addContact:
		case ServerMessageType::server_removeContact:
		case ServerMessageType::server_addIncomingContactRequest:
		case ServerMessageType::server_removeIncomingContactRequest:
		case ServerMessageType::server_addOutgoingContactRequest:
		case ServerMessageType::server_removeOutgoingContactRequest:
			stream << message.messageContents;
			break;
		case ServerMessageType::server_searchUserResult:
		case ServerMessageType::server_storedContacts:
		case ServerMessageType::server_storedIncomingContactRequests:
		case ServerMessageType::server_storedOutgoingContactRequests:
			stream << message.str_list;
			break;
		case ServerMessageType::server_userStateChanged:
			stream << message.string_state_pair;
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
	QPair<QString, UserState> string_state_pair;
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
		case ServerMessageType::server_addContact:
		case ServerMessageType::server_removeContact:
		case ServerMessageType::server_addIncomingContactRequest:
		case ServerMessageType::server_removeIncomingContactRequest:
		case ServerMessageType::server_addOutgoingContactRequest:
		case ServerMessageType::server_removeOutgoingContactRequest:
			stream >> messageContents;
			return Message(messageType, serverMessageType, dateTime, messageContents);
			break;
		case ServerMessageType::server_searchUserResult:
		case ServerMessageType::server_storedContacts:
		case ServerMessageType::server_storedIncomingContactRequests:
		case ServerMessageType::server_storedOutgoingContactRequests:
			stream >> str_list;
			return Message(messageType, serverMessageType, dateTime, str_list);
			break;
		case ServerMessageType::server_userStateChanged:
			stream >> string_state_pair;
			return Message(messageType, serverMessageType, dateTime, string_state_pair);
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
#include "chatbox_client.h"

Chatbox_Client::Chatbox_Client(QWidget* parent)
	:QWidget(parent)
	, socket(new QTcpSocket(this))
{
	setUpSignalSlotConnections();
}

void Chatbox_Client::setUpSignalSlotConnections()
{
	//socket-connections
	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(socket, SIGNAL(connected()), this, SLOT(connected()));
	connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(socketError()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(new_data_in_socket()));
}

void Chatbox_Client::new_data_in_socket()
{
	do {
		Message message = Message::readFromSocket(socket);
		handleMessage(message);
	} while (!socket->atEnd());
}

void Chatbox_Client::socketError()
{
	//show error in loading-screen
	if (current_scene == UI::Scene::loadingScene) {
		showLoadingSceneError();
	}
	//if the socket is not valid - act as if its disconnected
	else if (!socket->isValid())
		emit socket->disconnected();
}

void Chatbox_Client::showLoadingSceneError()
{
	emit setLoadingError(tr("Error establishing connection: %1").arg(socket->errorString()));
	//start cooldown-timer in mainWindow
	emit startLoadingTimer();
}

void Chatbox_Client::establishSocketConnection()
{
	//check if socket is actually unconnected
	if (socket->state() != QTcpSocket::UnconnectedState)
		return;

	//update loading-page
	emit setLoadingStatus("Establishing connection...");
	emit startLoadingPageAnimation();

	showLoadingScene();

	//try to establish socket connection
	socket->connectToHost(SERVER_IP, SERVER_PORT);
}

void Chatbox_Client::showLoadingScene()
{
	emit setScene(UI::Scene::loadingScene);
}

void Chatbox_Client::requestOlderMessages(QString chat_user_name, QDateTime dateTime)
{
	Message request = Message::createClientRequstMessage(dateTime, ClientRequestType::olderMessages, chat_user_name);
	Message::sendThroughSocket(socket, request);
}

void Chatbox_Client::searchUser(const QString& searchUser)
{
	Message request = Message::createClientRequstMessage(QDateTime::currentDateTime(), ClientRequestType::searchUserRequest, searchUser);
	Message::sendThroughSocket(socket, request);
}

void Chatbox_Client::addContact(const QString& contact)
{
	Message request = Message::createClientRequstMessage(QDateTime::currentDateTime(), ClientRequestType::addContact, contact);
	Message::sendThroughSocket(socket, request);
}

void Chatbox_Client::removeContact(const QString& contact)
{
	Message request = Message::createClientRequstMessage(QDateTime::currentDateTime(), ClientRequestType::removeContact, contact);
	Message::sendThroughSocket(socket, request);
}

void Chatbox_Client::sceneChanged(UI::Scene scene)
{
	current_scene = scene;
}

void Chatbox_Client::connected()
{
	emit setScene(UI::Scene::welcomeScene);
}

void Chatbox_Client::disconnected()
{
	emit clearLoginStatusLabel();
	emit clearRegistrationStatusLabel();
	emit stopWelcomePageAnimation();
	emit enableButtons();

	establishSocketConnection();
}

void Chatbox_Client::handleMessage(const Message& message)
{
	//login and registration - response from server
	if (current_scene == UI::Scene::welcomeScene) {
		if (message.getMessageType() == MessageType::server_message) {
			switch (message.getServerMessageType()) {
			case ServerMessageType::server_loginFailed:
				handleLoginFailedMessage(message); break;

			case ServerMessageType::server_loginSucceeded:
				handleLoginSucceededMessage(message); break;

			case ServerMessageType::server_registrationFailed:
				handleRegistrationFailedMessage(message); break;

			case ServerMessageType::server_registrationSucceeded:
				handleRegistrationSucceededMessage(); break;
			}
		}
	}
	else if (current_scene == UI::Scene::mainScene) {
		if (message.getMessageType() == MessageType::server_message) {
			switch (message.getServerMessageType())
			{
			case ServerMessageType::server_storedContacts:
				handleStoredContactsMessage(message); break;
			case ServerMessageType::server_storedIncomingContactRequests:
				handleStoredIncomingContactRequestsMessage(message); break;
			case ServerMessageType::server_storedOutgoingContactRequests:
				handleStoredOutgoingContactRequestsMessaage(message); break;
			case ServerMessageType::server_searchUserResult:
				handleSearchUserResultMessage(message); break;
			case ServerMessageType::server_addContact:
				handleAddContactMessage(message); break;
			case ServerMessageType::server_removeContact:
				handleRemoveContactMessage(message); break;
			case ServerMessageType::server_addIncomingContactRequest:
			case ServerMessageType::server_addOutgoingContactRequest:
				handleAddContactRequestMessage(message); break;
			case ServerMessageType::server_removeIncomingContactRequest:
			case ServerMessageType::server_removeOutgoingContactRequest:
				handleRemoveContactRequestMessage(message); break;
			case ServerMessageType::server_userStateChanged:
				handleUserStateChangedMessage(message); break;
			case ServerMessageType::server_noOlderMessagesAvailable:
				handleNoOlderMessagesAvailable(message); break;
			case ServerMessageType::server_errorMessage:
				handleErrorMessage(message); break;
			}
		}
		else if (message.getMessageType() == MessageType::chatMessage)
			handleChatMessage(message);
		else if (message.getMessageType() == MessageType::old_message)
			handleOldChatMessage(message);
	}
}

void Chatbox_Client::handleLoginFailedMessage(const Message& message)
{
	setLoginError(message.getContent());
	emit stopWelcomePageAnimation();
	emit clearLoginPasswordEdit();
	emit enableButtons();
}

void Chatbox_Client::handleLoginSucceededMessage(const Message& message)
{
	//delete all old items (e.g. contacts, contact_requests, chats...)
	emit clearUI();
	emit setScene(UI::Scene::mainScene);
	emit stopWelcomePageAnimation();
	emit enableButtons();
	emit clearLoginPasswordEdit();
	emit clearLoginStatusLabel();
	current_user_name = message.getContent();
	emit updateUserName(current_user_name);
}

void Chatbox_Client::handleRegistrationFailedMessage(const Message& message)
{
	emit stopWelcomePageAnimation();
	setRegistrationError(message.getContent());
	emit clearRegistrationPasswordEdit();
	emit enableButtons();
}

void Chatbox_Client::handleRegistrationSucceededMessage()
{
	emit stopWelcomePageAnimation();
	setRegistrationStatus("Successfully registered!");
	emit clearRegistrationPasswordEdit();
	emit enableButtons();
}

void Chatbox_Client::handleStoredContactsMessage(const Message& message)
{
	for (const auto& contact : message.getStringList())
		emit addContactSignal(contact);
}

void Chatbox_Client::handleStoredIncomingContactRequestsMessage(const Message& message)
{
	for (const auto& contact_request : message.getStringList())
		emit addContactRequest(contact_request, ServerMessageType::server_addIncomingContactRequest);
}

void Chatbox_Client::handleStoredOutgoingContactRequestsMessaage(const Message& message)
{
	for (const auto& contact_request : message.getStringList())
		emit addContactRequest(contact_request, ServerMessageType::server_addOutgoingContactRequest);
}

void Chatbox_Client::handleSearchUserResultMessage(const Message& message)
{
	emit searchUserResultsReceived(message.getStringList());
}

void Chatbox_Client::handleAddContactMessage(const Message& message)
{
	emit addContactSignal(message.getContent());
}

void Chatbox_Client::handleRemoveContactMessage(const Message& message)
{
	emit removeContactSignal(message.getContent());
}

void Chatbox_Client::handleAddContactRequestMessage(const Message& message)
{
	emit addContactRequest(message.getContent(), message.getServerMessageType());
}

void Chatbox_Client::handleRemoveContactRequestMessage(const Message& message)
{
	emit removeContactRequestSignal(message.getContent());
}

void Chatbox_Client::handleUserStateChangedMessage(const Message& message)
{
	emit userStateChanged(message.getStringStateContent());
}

void Chatbox_Client::handleNoOlderMessagesAvailable(const Message& message)
{
	emit noOlderMessagesAvailable(message.getContent());
}

void Chatbox_Client::handleErrorMessage(const Message& message)
{
	emit showMainWindowError(message.getContent());
}

void Chatbox_Client::handleChatMessage(const Message& message)
{
	emit chatMessageReceived(message);
}

void Chatbox_Client::handleOldChatMessage(const Message& message)
{
	emit oldChatMessageReceived(message, (message.getReceiver() == current_user_name) ? true : false);
}

void Chatbox_Client::handleLogin(const QString& username, const QString& unhashed_password)
{
	try {
		emit setLoginStatus("Encrypting your password ...");

		//hashes password -> might result in an Crypto_Error
		Message loginMessage = Message::createLoginMessage(QDateTime::currentDateTime(), username, unhashed_password);

		//queue message, so it gets send after thread execution
		queue_message(loginMessage);

		emit clearLoginPasswordEdit();

		emit setLoginStatus("Waiting on server response ...");
	}
	catch (CRYPTO::Crypto_Error& error) {
		//error occured in hash_function (e.g. not enough memory)
		showLoginError(error.get_error_message());
	}
}

void Chatbox_Client::showLoginError(QString error_msg)
{
	emit setLoginError(error_msg);

	emit enableButtons();

	//hide loading animation
	emit stopWelcomePageAnimation();
}

void Chatbox_Client::attemptLogin(const QString& username, const QString& unhashed_password)
{
	//create a thread to handleLogin (e.g. encrypting password)
	QThread* loginThread = QThread::create([&, username, unhashed_password] { handleLogin(username, unhashed_password); });
	//send all messages that were queued by the thread, after the thread has finished
	connect(loginThread, SIGNAL(finished()), this, SLOT(deliver_queued_messages()));
	loginThread->start();
}

void Chatbox_Client::handleRegistration(const QString& username, const QString& unencrypted_password)
{
	try {
		emit setRegistrationStatus("Encrypting your password ...");

		//hashes password -> might result in an Crypto_Error
		Message registrationMessage = Message::createRegistrationMessage(QDateTime::currentDateTime(), username, unencrypted_password);

		//queue message, so it gets send after thread execution
		queue_message(registrationMessage);

		emit clearRegistrationPasswordEdit();

		emit setRegistrationStatus("Waiting on server response ...");
	}
	catch (CRYPTO::Crypto_Error& error) {
#		//error occured in hash_function (e.g. not enough memory)
		showRegistrationError(error.get_error_message());

		return;
	}
}

void Chatbox_Client::showRegistrationError(QString error_msg)
{
	emit setRegistrationError(error_msg);

	emit enableButtons();

	//hide loading animation
	emit stopWelcomePageAnimation();
}

void Chatbox_Client::attemptRegistration(const QString& username, const QString& unencrypted_password)
{
	//create a thread to handleRegistration (e.g. encrypting password)
	QThread* registrationThread = QThread::create([&, username, unencrypted_password] { handleRegistration(username, unencrypted_password); });
	//send all messages that were queued by the thread, after the thread has finished
	connect(registrationThread, SIGNAL(finished()), this, SLOT(deliver_queued_messages()));
	registrationThread->start();
}

//stores queued messages from different threads, so that mainThread can safely send them through the socket
void Chatbox_Client::queue_message(Message message)
{
	QMutexLocker locker(&mutex);
	queued_messages.append(message);
}

//send queued messages through socket
void Chatbox_Client::deliver_queued_messages()
{
	QMutexLocker locker(&mutex);
	QList<Message>::iterator it = queued_messages.begin();
	while (it != queued_messages.end()) {
		Message::sendThroughSocket(socket, *it);
		it = queued_messages.erase(it);
	}
}
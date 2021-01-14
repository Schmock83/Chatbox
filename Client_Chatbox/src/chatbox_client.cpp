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
		emit setLoadingError(tr("Error establishing connection: %1").arg(socket->errorString()));
		//start cooldown-timer in mainWindow
		emit startLoadingTimer();
	}
	else if (!socket->isValid())
		emit socket->disconnected();
}

void Chatbox_Client::establishSocketConnection()
{
	if (socket->state() != QTcpSocket::UnconnectedState)
		return;

	//switch to loadingScene
	emit setScene(UI::Scene::loadingScene);

	//update labels
	emit clearLoadingErrorLabel();
	emit setLoadingStatus("Establishing connection...");

	emit startLoadingPageAnimation();

	//try to establish socket connection
	socket->connectToHost(SERVER_IP, SERVER_PORT);
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
	qDebug() << "handleMessage called: ";
	message.print();
	//login and registration - response from server
	if (current_scene == UI::Scene::welcomeScene) {
		if (message.getMessageType() == MessageType::server_message) {
			switch (message.getServerMessageType()) {
			case ServerMessageType::server_loginFailed:
				setLoginError(message.getContent());
				emit stopWelcomePageAnimation();
				emit clearLoginPasswordEdit();
				emit enableButtons();
				break;

			case ServerMessageType::server_loginSucceeded:
				//delete all old items (e.g. contacts, contact_requests, chats...)
				emit clearUI();
				emit setScene(UI::Scene::mainScene);
				emit stopWelcomePageAnimation();
				emit enableButtons();
				emit clearLoginPasswordEdit();
				emit clearLoginStatusLabel();
				break;

			case ServerMessageType::server_registrationFailed:
				emit stopWelcomePageAnimation();
				setRegistrationError(message.getContent());
				emit clearRegistrationPasswordEdit();
				emit enableButtons();
				break;

			case ServerMessageType::server_registrationSucceeded:
				emit stopWelcomePageAnimation();
				setRegistrationStatus("Successfully registered!");
				emit clearRegistrationPasswordEdit();
				emit enableButtons();
				break;
			}
		}
	}
	else if (current_scene == UI::Scene::mainScene) {
		if (message.getMessageType() == MessageType::server_message) {
			switch (message.getServerMessageType())
			{
			case ServerMessageType::server_storedContacts:
				for (const auto& contact : message.getStringList())
					emit addContactSignal(contact);
				break;
			case ServerMessageType::server_storedIncomingContactRequests:
				for (const auto& contact_request : message.getStringList())
					emit addContactRequestSignal(contact_request, ServerMessageType::server_addIncomingContactRequest);
				break;
			case ServerMessageType::server_storedOutgoingContactRequests:
				for (const auto& contact_request : message.getStringList())
					emit addContactRequestSignal(contact_request, ServerMessageType::server_addOutgoingContactRequest);
				break;
			case ServerMessageType::server_searchUserResult:
				emit searchUsersSignal(message.getStringList());
				break;
			case ServerMessageType::server_addContact:
				emit addContactSignal(message.getContent());
				break;
			case ServerMessageType::server_removeContact:
				emit removeContactSignal(message.getContent());
				break;
			case ServerMessageType::server_addIncomingContactRequest:
			case ServerMessageType::server_addOutgoingContactRequest:
				emit addContactRequestSignal(message.getContent(), message.getServerMessageType());
				break;
			case ServerMessageType::server_removeIncomingContactRequest:
			case ServerMessageType::server_removeOutgoingContactRequest:
				emit removeContactRequestSignal(message.getContent());
				break;
			case ServerMessageType::server_userStateChanged:
				emit userStateChanged(message.getStringStateContent());
				break;
			}
		}
	}
}

void Chatbox_Client::handleLogin(const QString& username, const QString& unhashed_password)
{
	emit clearRegistrationStatusLabel();
	emit setLoginStatus("Encrypting your password ...");
	try {
		//hashes password -> might result in an Crypto_Error
		Message loginMessage = Message::createLoginMessage(QDateTime::currentDateTime(), username, unhashed_password);
		//queue message, so it gets send after thread execution
		queue_message(loginMessage);
	}
	catch (CRYPTO::Crypto_Error& error) {
#		//error occured in hash_function (e.g. not enough memory) -> print errorMessage in label
		emit setLoginError(error.get_error_message());

		emit enableButtons();

		//hide loading animation
		emit stopWelcomePageAnimation();

		//exit
		return;
	}

	emit clearLoginPasswordEdit();

	emit setLoginStatus("Waiting on server response ...");
}

void Chatbox_Client::attemptLogin(const QString& username, const QString& unhashed_password)
{
	QThread* loginThread = QThread::create([&, username, unhashed_password] { handleLogin(username, unhashed_password); });
	connect(loginThread, SIGNAL(finished()), this, SLOT(deliver_queued_messages()));
	loginThread->start();
}

void Chatbox_Client::handleRegistration(const QString& username, const QString& unencrypted_password)
{
	emit clearLoginStatusLabel();
	emit setRegistrationStatus("Encrypting your password ...");

	try {
		//hashes password -> might result in an Crypto_Error
		Message registrationMessage = Message::createRegistrationMessage(QDateTime::currentDateTime(), username, unencrypted_password);
		//send message through socket
		//queue message, so it gets send after thread execution
		queue_message(registrationMessage);
	}
	catch (CRYPTO::Crypto_Error& error) {
#		//error occured in hash_function (e.g. not enough memory) -> print errorMessage in label
		emit setRegistrationError(error.get_error_message());

		emit enableButtons();

		//hide loading animation
		emit stopWelcomePageAnimation();

		//exit
		return;
	}

	emit clearRegistrationPasswordEdit();

	emit setRegistrationStatus("Waiting on server response ...");
}

void Chatbox_Client::attemptRegistration(const QString& username, const QString& unencrypted_password)
{
	QThread* registrationThread = QThread::create([&, username, unencrypted_password] { handleRegistration(username, unencrypted_password); });
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
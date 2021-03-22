#include "chatbox_client.h"

#include "../core_includes/messages/messagewrapper.h"

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
        Base_Message* message = MessageWrapper::readMessageFromSocket(socket);
        message->handleOnClientSide(this);
	} while (!socket->atEnd());
}

void Chatbox_Client::socketError()
{
	//show error in loading-screen
	if (current_scene == UI::Scene::loadingScene) {
        emit setLoadingError(tr("Error establishing connection: %1").arg(socket->errorString()));
        //start cooldown-timer in mainWindow
        emit startLoadingTimer();

        //showLoadingSceneError();
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
    Base_Message* request_Message = new Send_Older_Messages_Request(chat_user_name, dateTime);
    MessageWrapper::sendMessageThroughSocket(socket, request_Message);
}

void Chatbox_Client::searchUser(const QString& searchUser)
{
    Base_Message* request_Message = new Search_User_Request_Message(searchUser);
    MessageWrapper::sendMessageThroughSocket(socket, request_Message);
}

void Chatbox_Client::addContact(const QString& contact)
{
    Base_Message* request_Message = new Add_Contact_Request_Message(contact);
    MessageWrapper::sendMessageThroughSocket(socket, request_Message);
}

void Chatbox_Client::removeContact(const QString& contact)
{
    Base_Message* request_Message = new Remove_Contact_Request_Message(contact);
    MessageWrapper::sendMessageThroughSocket(socket, request_Message);
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

void Chatbox_Client::handleLogin(const QString& username, const QString& unhashed_password)
{
	try {
		emit setLoginStatus("Encrypting your password ...");

		//hashes password -> might result in an Crypto_Error
        Login_Request_Message* login_Message = new Login_Request_Message(username, unhashed_password);
        login_Message->hashPassword();

		//queue message, so it gets send after thread execution
        queue_message(login_Message);

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
        Registration_Request_Message* registration_Message = new Registration_Request_Message(username, unencrypted_password);
        registration_Message->encryptPassword();

		//queue message, so it gets send after thread execution
        queue_message(registration_Message);

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
void Chatbox_Client::queue_message(Base_Message* message)
{
	QMutexLocker locker(&mutex);
	queued_messages.append(message);
}

//send queued messages through socket
void Chatbox_Client::deliver_queued_messages()
{
	QMutexLocker locker(&mutex);
    QList<Base_Message*>::iterator it = queued_messages.begin();
	while (it != queued_messages.end()) {
        MessageWrapper::sendMessageThroughSocket(socket, *it);
		it = queued_messages.erase(it);
	}
}

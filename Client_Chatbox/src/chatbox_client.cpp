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
	establishSocketConnection();
}

void Chatbox_Client::handleLogin(const QString& username, const QString& unhashed_password)
{
	emit setLoginStatus("Encrypting your password ...");
	try {
		//hashes password -> might result in an Crypto_Error
		Message loginMessage = Message::createLoginMessage(QDateTime::currentDateTime(), username, unhashed_password);
		//send message through socket
		Message::sendThroughSocket(socket, loginMessage);
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

	emit enableButtons();

	//clear password-field and statusLabels
	emit clearLoginPasswordEdit();
	emit clearLoginStatusLabel();

	//hide loading animation
	emit stopWelcomePageAnimation();
}

void Chatbox_Client::attemptLogin(const QString& username, const QString& unhashed_password)
{
	QThread* loginThread = QThread::create([&, username, unhashed_password] { handleLogin(username, unhashed_password); });
	loginThread->start();
}

void Chatbox_Client::handleRegistration(const QString& username, const QString& unencrypted_password)
{
	emit setRegistrationStatus("Encrypting your password ...");

	try {
		//hashes password -> might result in an Crypto_Error
		Message registrationMessage = Message::createRegistrationMessage(QDateTime::currentDateTime(), username, unencrypted_password);
		//send message through socket
		Message::sendThroughSocket(socket, registrationMessage);
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

	emit enableButtons();

	//clear password-field and statusLabels
	emit clearRegistrationPasswordEdit();
	emit clearRegistrationStatusLabel();

	//hide loading animation
	emit stopWelcomePageAnimation();
}

void Chatbox_Client::attemptRegistration(const QString& username, const QString& unencrypted_password)
{
	QThread* registrationThread = QThread::create([&, username, unencrypted_password] { handleRegistration(username, unencrypted_password); });
	registrationThread->start();
}
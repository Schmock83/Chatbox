#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, socket(new QTcpSocket(this))
{
	ui->setupUi(this);

	setUpUi();

	setUpSignalSlotConnections();

	establishSocketConnection();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::establishSocketConnection()
{
	//update labels
	ui->loadingErrorLabel->clear();
	ui->loadingStatusLabel->setText(tr("Establishing connection..."));

	//show loading screen
	ui->sceneWidget->setCurrentIndex(Scene::loadingScene);
	loadingAnimation->start();

	//try to establish socket connection
	socket->connectToHost(SERVER_IP, SERVER_PORT);
}

void MainWindow::setUpUi()
{
	ui->chats_grid_layout->setAlignment(Qt::AlignTop);
	ui->contacts_grid_layout->setAlignment(Qt::AlignTop);

	//initiate loading animation
	loadingAnimation = new QMovie(":/loadingGifs/imgs/loading3.gif");
	ui->gif_label->setMovie(loadingAnimation);
	loadingAnimation->start();

	UI::setUpDarkTheme();

	ui->chat_contacts_stackedWidget->setCurrentIndex(ChatContactPage::chatPage);
}

void MainWindow::setUpSignalSlotConnections()
{
	//socket-connections
	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(socket, SIGNAL(connected()), this, SLOT(connected()));
	connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(socketError()));

	//buttons
	connect(ui->login_Button, SIGNAL(clicked()), this, SLOT(attemptLogin()));
	connect(ui->registration_Button, SIGNAL(clicked()), this, SLOT(attemptRegistration()));
}

void MainWindow::disconnected()
{
	establishSocketConnection();
}

void MainWindow::connected()
{
	ui->sceneWidget->setCurrentIndex(Scene::welcomeScene);
}

void MainWindow::socketError()
{
	qDebug() << "SocketError...";
	//show error in loading-screen
	if (ui->sceneWidget->currentIndex() == Scene::loadingScene) {
		ui->loadingErrorLabel->setText(tr("Error establishing connection: %1").arg(socket->errorString()));
		checkIfSocketConnected();
	}
	else if (!socket->isValid())
		emit socket->disconnected();
}

/*void MainWindow::on_pushButton_clicked()
{
	Message m1("Ingrid", 53, 133.70);
		m1.print();

		QDataStream in(&socket);
		in << m1;
		socket.flush();
		qDebug() << "Send...";
}*/

//puts newTopButton to the front of chatButtons-list (adds or re-adds if already in list)
void MainWindow::addTopChatButton(QPushButton* newTopButton)
{
	//remove if exists
	chatButtons.remove(newTopButton);

	//re-add to front
	chatButtons.push_front(newTopButton);
}

void MainWindow::updateChatList()
{
	//delete all buttons from layout
	QLayoutItem* wItem;
	while ((wItem = ui->chats_grid_layout->layout()->takeAt(0)) != NULL)
		delete wItem;

	//re-add them
	for (const auto& button : chatButtons)
		ui->chats_grid_layout->addWidget(button);
}

#pragma region LOADING-SCENE
void MainWindow::checkIfSocketConnected()
{
	if (socket->state() != QTcpSocket::ConnectedState) {
		//wait 6 seconds and then call establishSocketConnection again...
		cooldown_timer = cooldown_secs;

		//timer for every second to update text
		updateLoadingScreenLabel();
	}
}

void MainWindow::updateLoadingScreenLabel()
{
	ui->loadingStatusLabel->setText(tr("Trying again in %2 seconds...").arg(QString::number(cooldown_timer)));
	cooldown_timer--;
	if (cooldown_timer >= 0)
		QTimer::singleShot(1000, this, SLOT(updateLoadingScreenLabel()));
	else
		establishSocketConnection();
}
#pragma endregion LOADING-SCENE

//TODO hash + encrypt password
//for login and registration
#pragma region WELCOME-SCENE
void MainWindow::attemptLogin()
{
	qDebug() << "attempting login...";

	//hide/clear errorLabels
	ui->login_errorLabel->clear();
	ui->registration_errorLabel->clear();

	//check if username and password were entered and are long enough
	if (ui->login_UsernameEdit->text().length() < 5) {
		ui->login_errorLabel->setText("<div style='color: red'>The Username has to be at least 5 Characters long !</div>");
		return;
	}
	else if (ui->login_PasswordEdit->text().length() < 8) {
		ui->login_errorLabel->setText("<div style='color: red'>The Pasword has to be at least 8 Characters long !</div>");
		return;
	}

	QString username = ui->login_UsernameEdit->text();
	QString password = ui->login_PasswordEdit->text();
	Message loginMessage = Message::createLoginMessage(QDateTime::currentDateTime(), username, password);

	//send message through socket
	Message::sendThroughSocket(socket, loginMessage);

	//clear password-field
	ui->login_PasswordEdit->clear();
}

void MainWindow::attemptRegistration()
{
	qDebug() << "attempt registration...";

	//hide/clear errorLabels
	ui->login_errorLabel->clear();
	ui->registration_errorLabel->clear();

	//check if username and password were entered and are long enough
	if (ui->registration_UsernameEdit->text().length() < 5) {
		ui->registration_errorLabel->setText("<div style='color: red'>The Username has to be at least 5 Characters long !</div>");
		return;
	}
	else if (ui->registration_PasswordEdit->text().length() < 8) {
		ui->registration_errorLabel->setText("<div style='color: red'>The Pasword has to be at least 8 Characters long !</div>");
		return;
	}

	QString username = ui->registration_UsernameEdit->text();
	QString password = ui->registration_PasswordEdit->text();
	Message loginMessage = Message::createRegistrationMessage(QDateTime::currentDateTime(), username, password);

	//send message through socket
	Message::sendThroughSocket(socket, loginMessage);

	//clear password-field
	ui->registration_PasswordEdit->clear();
}
#pragma endregion WELCOME-SCENE
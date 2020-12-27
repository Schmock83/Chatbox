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

	//establishSocketConnection();
	for (int i = 0; i < 32; i++) {
		QPushButton* push_btn = new QPushButton(tr("Chat - %1").arg(QString::number(i + 1)));
		ui->chats_grid_layout->addWidget(push_btn);
	}

	ui->contacts_grid_layout->addWidget(new QPushButton("Contact 123"));

    ui->sceneWidget->setCurrentIndex(Scene::loadingScene);
}

void MainWindow::establishSocketConnection()
{
	/*if(socket->state() == QTcpSocket::ConnectedState){
		qDebug() << "nothing to do in establishSocketCOnnection";
		return;
	}*/

	qDebug() << "establishingSOcketConnection called...";
	//update labels
	ui->loadingErrorLabel->clear();
	ui->loadingStatusLabel->setText(tr("Establishing connection..."));

	//show loading screen
	ui->sceneWidget->setCurrentIndex(Scene::loadingScene);
	loadingAnimation->start();

	//try to establish socket connection
	socket->connectToHost(SERVER_IP, SERVER_PORT);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::socketError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "socket-error called";
	//show error in loading-screen
	if (ui->sceneWidget->currentIndex() == Scene::loadingScene) {
		ui->loadingErrorLabel->setText(tr("Error establishing connection: %1").arg(socket->errorString()));
		checkIfSocketConnected();
	}

	//TODO handle errors
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
	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(socket, SIGNAL(connected()), this, SLOT(connected()));
	connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
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

void MainWindow::disconnected()
{
	establishSocketConnection();
}

void MainWindow::connected()
{
	ui->sceneWidget->setCurrentIndex(Scene::welcomeScene);
}

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

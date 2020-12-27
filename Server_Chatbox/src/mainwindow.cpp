#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(&server, SIGNAL(newConnection()), this, SLOT(newConnection()));

	if (!server.listen(QHostAddress::Any, 1338)) {
		qDebug() << "!!!!Cant start listening: " << server.errorString();
	}
	else {
		qDebug() << "Listening on port 1338";
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::newConnection()
{
	qDebug() << "newConnection";
	QTcpSocket* clientConnection = server.nextPendingConnection();
	connect(clientConnection, SIGNAL(readyRead()), this, SLOT(readNewData()));
}

void MainWindow::readNewData()
{
	qDebug() << "received new Data";
	QTcpSocket* clientSocket = (QSslSocket*)sender();

	QDataStream in(clientSocket);
	Message msg;;
	in >> msg;

	msg.print();
}
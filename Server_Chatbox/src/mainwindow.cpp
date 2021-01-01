#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	server = new Chatbox_Server(this);
}

bool MainWindow::initialize_server()
{
	try {
		server->startServer(QHostAddress::Any, 1338);
		return true;
	}
	catch (Server_Error& server_error) {
		QMessageBox::information(this, "Server Error", "Could not start Server: " + server_error.get_error_message());
		return false;
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}
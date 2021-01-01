#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>

#include <sodium.h>

#include "databasehelper.h"
#include <QHash>
#include "user.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	//check if libsodium was initialized properly
	if (sodium_init() < 0) {
		QMessageBox::information(nullptr, "Library Error", "Sodium library could not be initialized properly!");
		return -1;
	}

	MainWindow w;
	//check if server was started properly:
	if (!w.initialize_server()) {
		return -2;
	}
	w.show();
	return a.exec();
}
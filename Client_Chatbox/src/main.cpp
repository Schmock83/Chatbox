#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>

#include <sodium.h>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	if (sodium_init() < 0) {
		qDebug() << "HI";
		QMessageBox::information(nullptr, "Library Error", "Sodium library could not be initialized properly!");
		return -1;
	}

	MainWindow w;
	w.show();
	return a.exec();
}
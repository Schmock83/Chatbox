#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTcpServer>
#include <QTcpSocket>
#include <QSslSocket>

#include <QByteArray>
#include <QDataStream>

#include "../core_includes/message.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();
private slots:
	void newConnection();
	void readNewData();
private:
	Ui::MainWindow* ui;
	QTcpServer server;
	//MySslServer server;
};
#endif // MAINWINDOW_H

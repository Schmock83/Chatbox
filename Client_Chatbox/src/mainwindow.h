#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTcpSocket>
#include <QDataStream>
#include <QPushButton>
#include <QMovie>
#include <QTimer>

#include "../core_includes/message.h"
#include "../core_includes/core.h"

enum Scene {
	loadingScene = 0,
	welcomeScene,
	mainScene
};

//left side (show contacts or chats)
enum ChatContactPage {
	chatPage = 0,
	contactPage
};

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
	void socketError(QAbstractSocket::SocketError);
	void disconnected();
	void connected();
	void checkIfSocketConnected();
	void updateLoadingScreenLabel();

private:
	Ui::MainWindow* ui;
	QTcpSocket* socket;
	QMovie* loadingAnimation;
	std::list<QPushButton*> chatButtons;

	//for timer in loadingScene
	const int cooldown_secs = 6;
	int cooldown_timer = cooldown_secs;

	void setUpUi();
	void setUpSignalSlotConnections();
	void addTopChatButton(QPushButton*);
	void updateChatList();
	void establishSocketConnection();
};
#endif // MAINWINDOW_H

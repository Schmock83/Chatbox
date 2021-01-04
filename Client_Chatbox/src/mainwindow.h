#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMovie>
#include <QTimer>
#include <QThread>
#include <QMap>
#include <QHash>
#include <QList>

#include "../core_includes/core.h"
#include "chatbox_client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT
private:
	Ui::MainWindow* ui;
	Chatbox_Client* client;
	QMovie* loadingAnimation;
	std::list<QPushButton*> chatButtons;
	QMap<QChar, QMap<QString, QPushButton*>> contacts;

	//for timer in loadingScene
	const int cooldown_secs = 6;
	int cooldown_timer = cooldown_secs;

	void setUpUi();
	void setUpSignalSlotConnections();
	void addTopChatButton(QPushButton*);
	void updateChatList();
	void updateContactList();
	void deleteWidgetsFromLayout(QLayout*);

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

signals:
	void establishSocketConnection_signal();
	void sceneChanged(UI::Scene);

private slots:
	void addContact(QString contact);
	void setScene(UI::Scene scene);
	void setLoadingStatus(QString new_status);
	void setLoadingError(QString new_error);
	void setRegistrationStatus(QString new_status);
	void setRegistrationError(QString new_error);
	void setLoginStatus(QString new_status);
	void setLoginError(QString new_error);
	void enableButtons();
	void disableButtons();
	void startLoadingPageAnimation();
	void startWelcomePageAnimation();
	void stopLoadingPageAnimation();
	void stopWelcomePageAnimation();
	void clearLoadingErrorLabel();
	void clearLoginStatusLabel();
	void clearRegistrationStatusLabel();
	void clearLoginPasswordEdit();
	void clearRegistrationPasswordEdit();
	void startLoadingTimer();
	void updateLoadingScreenLabel();

	void login_button_pressed();
	void registration_button_pressed();
};
#endif // MAINWINDOW_H

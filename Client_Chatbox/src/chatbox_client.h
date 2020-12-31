#ifndef CHATBOX_CLIENT_H
#define CHATBOX_CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QThread>

#include "../core_includes/message.h"
#include "../core_includes/core.h"

class Chatbox_Client : public QWidget
{
	Q_OBJECT
private:
	QTcpSocket* socket;
	UI::Scene current_scene;

	void handleLogin(const QString& username, const QString& unhashed_password);	//called by Mainwindow
	void handleRegistration(const QString& username, const QString& unencrypted_password); //called by MainWindow

public:
	Chatbox_Client(QWidget* parent = nullptr);
	void setUpSignalSlotConnections();
	void attemptLogin(const QString& username, const QString& unhashed_password);
	void attemptRegistration(const QString& username, const QString& unencrypted_password);

public slots:
	//socket-handling
	void establishSocketConnection();
private slots:
	//socket-handling
	void disconnected();
	void connected();
	void socketError();
	void sceneChanged(UI::Scene scene);

signals:
	void setScene(UI::Scene);
	void enableButtons();
	void disableButtons();
	void startLoadingPageAnimation();
	void stopLoadingPageAnimation();
	void startWelcomePageAnimation();
	void stopWelcomePageAnimation();
	void clearLoadingErrorLabel();
	void clearLoginPasswordEdit();
	void clearLoginStatusLabel();
	void clearRegistrationStatusLabel();
	void clearRegistrationPasswordEdit();
	void setLoadingStatus(QString new_status);
	void setLoadingError(QString new_error);
	void setRegistrationStatus(QString new_status);
	void setRegistrationError(QString new_error);
	void setLoginStatus(QString new_status);
	void setLoginError(QString new_error);
	void startLoadingTimer();
};

#endif // CHATBOX_CLIENT_H

#ifndef CHATBOX_CLIENT_H
#define CHATBOX_CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QThread>
#include <QMutex>

#include "../core_includes/message.h"
#include "../core_includes/core.h"

class Chatbox_Client : public QWidget
{
	Q_OBJECT
private:
	QTcpSocket* socket;
	UI::Scene current_scene;
	QList <Message> queued_messages;
	mutable QMutex mutex;

	void handleLogin(const QString& username, const QString& unhashed_password);	//called by Mainwindow
	void handleRegistration(const QString& username, const QString& unencrypted_password); //called by MainWindow
	void handleMessage(const Message& message);
	void queue_message(Message message);	//queue´s messages from threads, so that the main thread can safely send them through the socket
public:
	Chatbox_Client(QWidget* parent = nullptr);
	void setUpSignalSlotConnections();
	void attemptLogin(const QString& username, const QString& unhashed_password);
	void attemptRegistration(const QString& username, const QString& unencrypted_password);

public slots:
	//socket-handling
	void establishSocketConnection();

	void searchUser(const QString&);
	void addContact(const QString&);
	void removeContact(const QString&);
private slots:
	//socket-handling
	void disconnected();
	void connected();
	void socketError();
	void new_data_in_socket();
	void deliver_queued_messages();

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
	void searchUsersSignal(QList<QString>);
	void addContactSignal(const QString&);
	void removeContactSignal(const QString&);
	void addContactRequestSignal(const QString&, ServerMessageType);
	void removeContactRequestSignal(const QString&);
	void clearUI();
	void userStateChanged(QPair<QString, UserState>);
};

#endif // CHATBOX_CLIENT_H

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
	QString current_user_name;

	void handleLogin(const QString& username, const QString& unhashed_password);	//called by Mainwindow
	void showLoginError(QString error_msg);
	void handleRegistration(const QString& username, const QString& unencrypted_password); //called by MainWindow
	void showRegistrationError(QString error_msg);
	void handleMessage(const Message& message);
	void handleLoginFailedMessage(const Message& message);
	void handleLoginSucceededMessage(const Message& message);
	void handleRegistrationFailedMessage(const Message& message);
	void handleRegistrationSucceededMessage();
	void handleStoredContactsMessage(const Message& message);
	void handleStoredIncomingContactRequestsMessage(const Message& message);
	void handleStoredOutgoingContactRequestsMessaage(const Message& message);
	void handleSearchUserResultMessage(const Message& message);
	void handleAddContactMessage(const Message& message);
	void handleRemoveContactMessage(const Message& message);
	void handleAddContactRequestMessage(const Message& message);
	void handleRemoveContactRequestMessage(const Message& message);
	void handleUserStateChangedMessage(const Message& message);
	void handleNoOlderMessagesAvailable(const Message& message);
	void handleErrorMessage(const Message& message);
	void handleChatMessage(const Message& message);
	void handleOldChatMessage(const Message& message);
	void showLoadingScene();
public:
	Chatbox_Client(QWidget* parent = nullptr);
	void setUpSignalSlotConnections();
	void attemptLogin(const QString& username, const QString& unhashed_password);
	void attemptRegistration(const QString& username, const QString& unencrypted_password);
	void queue_message(Message message);	//queue´s messages from threads, so that the main thread can safely send them through the socket
public slots:
	void establishSocketConnection();
	void deliver_queued_messages();
	void searchUser(const QString&);
	void addContact(const QString&);
	void removeContact(const QString&);
	void requestOlderMessages(QString, QDateTime);
private slots:
	//socket-handling
	void disconnected();
	void connected();
	void socketError();
	void showLoadingSceneError();
	void new_data_in_socket();

	void sceneChanged(UI::Scene scene);

signals:
	void updateUserName(const QString&);
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
	void showMainWindowError(QString);
	void startLoadingTimer();
	void searchUserResultsReceived(QList<QString>);
	void addContactSignal(const QString&);
	void removeContactSignal(const QString&);
	void addContactRequest(const QString&, ServerMessageType);
	void removeContactRequestSignal(const QString&);
	void clearUI();
	void userStateChanged(QPair<QString, UserState>);
	void chatMessageReceived(const Message&);
	void oldChatMessageReceived(const Message&, bool);
	void noOlderMessagesAvailable(QString);
};

#endif // CHATBOX_CLIENT_H

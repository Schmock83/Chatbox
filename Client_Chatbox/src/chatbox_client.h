#ifndef CHATBOX_CLIENT_H
#define CHATBOX_CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QThread>
#include <QMutex>
#include <QDateTime>

#include "../core_includes/core.h"

class Base_Message;
class Client_Chat_Message;

class Chatbox_Client : public QWidget
{
    Q_OBJECT
private:
    QTcpSocket* socket;
    UI::Scene current_scene;
    QList <Base_Message*> queued_messages;
    mutable QMutex mutex;
    QString current_user_name;

    void handleLogin(const QString& username, const QString& unhashed_password);
    void showLoginError(QString error_msg);
    void handleRegistration(const QString& username, const QString& unencrypted_password);
    void showRegistrationError(QString error_msg);
    void handleLoginSucceededMessage();
    void handleRegistrationFailedMessage();
    void handleRegistrationSucceededMessage();
    void handleStoredContactsMessage();
    void handleStoredIncomingContactRequestsMessage();
    void handleStoredOutgoingContactRequestsMessaage();
    void handleSearchUserResultMessage();
    void handleAddContactMessage();
    void handleRemoveContactMessage();
    void handleAddContactRequestMessage();
    void handleRemoveContactRequestMessage();
    void handleUserStateChangedMessage();
    void handleNoOlderMessagesAvailable();
    void handleErrorMessage();
    void handleChatMessage();
    void showLoadingScene();
public:
    Chatbox_Client(QWidget* parent = nullptr);
    void setUpSignalSlotConnections();
    void attemptLogin(const QString& username, const QString& unhashed_password);
    void attemptRegistration(const QString& username, const QString& unencrypted_password);
    void queue_message(Base_Message* message);	//queue`s messages from threads, so that the main thread can safely send them through the socket
    void setCurrentUserName(QString user_name) {this->current_user_name = user_name;}
    QString getCurrentUserName() {return this->current_user_name;}
    QTcpSocket* getSocket() {return socket;}
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
    void addContactRequest(const QString&, ContactRequestType);
    void removeContactRequestSignal(const QString&);
    void clearUI();
    void userStateChanged(QPair<QString, UserState>);
    void chatMessageReceived(Client_Chat_Message* chat_Message, QString current_Username);
    void noOlderMessagesAvailable(QString);
};

#endif // CHATBOX_CLIENT_H

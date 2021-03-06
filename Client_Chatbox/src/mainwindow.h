#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMovie>
#include <QTimer>
#include <QThread>
#include <QMap>
#include <QHash>
#include <QList>
#include <QVBoxLayout>
#include <QPixmap>
#include <QDateTime>
#include <QLineEdit>

#include "../core_includes/core.h"
#include "chatbox_client.h"
#include "widgets/userbutton.h"
#include "widgets/chatbrowser.h"
#include "widgets/fading_information_box.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT
private:
	Ui::MainWindow* ui;
	Fading_Information_Box* information_box = nullptr;
	Chatbox_Client* client;
	QMovie* loadingAnimation;
	QHash<QString, QWidget*> chatWindows;
	std::list<UserButton*> chatButtons;
	QMap<QChar, QMap<QString, UserButton*>> contacts;
	QMap<QString, UserButton*> contact_requests; //to store open (incoming or outgoing) contact-requests from other users

	//for keeping track of wich left-side page was selected before clicking on lineedit
	UI::ChatContactPage previous_left_page = UI::ChatContactPage::contactPage;

	//for timer in loadingScene
	const int cooldown_secs = 6;
	int cooldown_timer = cooldown_secs;

	void initializeUI();
	void setUpSignalSlotConnections();
	void addTopChatButton(UserButton*);
	void updateChatList();
	void updateContactList();
	void updateSearchedUser(const QString&, bool = false, bool = false, bool = false);
    void updateSearchedUser(const QString&, ContactRequestType);
	void updateChatButton(const QString&, bool = false, bool = false, bool = false);
    void updateChatButton(const QString&, ContactRequestType);
	void deleteWidgetsFromLayout(QLayout*, bool = true);
	bool isContact(const QString&);
	bool hasIncomingContactRequest(const QString&);
	bool hasOutgoingContactRequest(const QString&);
	UserButton* getContactRequestButton(const QString&);
	int getChatWindowIndex(const QString&);
	int buildChatWindow(const QString&);	//creates a chat(Browser)-window for a given username
	ChatBrowser* getChatForIndex(int index);
	UserButton* getChatButton(const QString&);
	UserButton* addChatButton(const QString&);
	virtual void resizeEvent(QResizeEvent* event);
    void showOnlineIcon();
    void repositionInformationBox();
    QWidget* getCurrentChatWindow();
    QLineEdit* getCurrentLineEdit();
public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

signals:
	void establishSocketConnection_signal();
	void sceneChanged(UI::Scene);
	void searchUserSignal(const QString&);
	void addContactSignal(const QString&);

private slots:
	void setUserNameLabel(const QString&);
	void showPopupInformationBox(const QString& str);
    void appendToChatHistory(QString chat_user_name, Client_Chat_Message* chat_Message);
	void sendMessage();
	void showChatWindow(const QString&);
	void clearUI();
	void on_chat_button_clicked();
	void on_contacts_button_clicked();
	void addContact(const QString& contact);
	void removeContact(const QString& contact);
    void addContactRequest(const QString& contact, ContactRequestType requestType);
	void removeContactRequest(const QString& contact);
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
	void search_line_edit_focussed();
	void search_line_edit_unfocussed();
	void search_line_edit_returnPressed();
	void addSearchUserResults(QList<QString>);
	void login_button_pressed();
	void registration_button_pressed();
	void userStateChanged(QPair<QString, UserState>);
    void chatMessageReceived(Client_Chat_Message* chat_Message, QString current_Username);
	void noOlderMessagesAvailable(QString);
};
#endif // MAINWINDOW_H

#ifndef USERBUTTON_H
#define USERBUTTON_H

//Overloaded QPushButton, to display other users in the ui + show if they`re online/offline

#include <QObject>
#include <QPushButton>
#include <QMouseEvent>
#include <QMenu>

#include "../core_includes/core.h"

class UserButton : public QPushButton
{
	Q_OBJECT
public:
	UserButton(const QString& text, bool is_contact = false, bool incoming_contact_request = false, bool outgoing_contact_request = false, QWidget* parent = nullptr);
	UserButton(const QIcon& icon, const QString& text, bool is_contact = false, bool incoming_contact_request = false, bool outgoing_contact_request = false, QWidget* parent = nullptr);
	bool is_contact;
	bool incoming_contact_request;
	bool outgoing_contact_request;
    void setFlags(ContactRequestType requestType);
	void setFlags(bool is_contact = false, bool incoming_contact_request = false, bool outgoing_contact_request = false) { this->is_contact = is_contact; this->incoming_contact_request = incoming_contact_request; this->outgoing_contact_request = outgoing_contact_request; }
	void setState(UserState newState);
	void messageReceived();
	virtual QString text() { return original_text; }
private:
	virtual void mousePressEvent(QMouseEvent* e);
	UserState userState = UserState::unknown;
	QString original_text;
	int unreadMessageCount = 0;
    void showContactContextMenu(QMouseEvent* e);
    void showIncomingContactRequestContextMenu(QMouseEvent* e);
    void showOutgoingContactRequestContextMenu(QMouseEvent* e);
    void showDefaultContextMenu(QMouseEvent* e);
    void showDefaultContextMessage(QMouseEvent* e);

private slots:
	void removeContactClicked();
	void addContactClicked();
signals:
	void removeContact(const QString&);
	void addContact(const QString&);
	void userbutton_clicked(const QString&);
};

#endif // USERBUTTON_H

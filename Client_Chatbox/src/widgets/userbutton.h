#ifndef USERBUTTON_H
#define USERBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QMouseEvent>
#include <QMenu>

#include <QDebug>

#include "../core_includes/message.h"

class UserButton : public QPushButton
{
	Q_OBJECT
public:
	UserButton(const QString& text, bool is_contact = false, bool incoming_contact_request = false, bool outgoing_contact_request = false, QWidget* parent = nullptr);
	UserButton(const QIcon& icon, const QString& text, bool is_contact = false, bool incoming_contact_request = false, bool outgoing_contact_request = false, QWidget* parent = nullptr);
	bool is_contact;
	bool incoming_contact_request;
	bool outgoing_contact_request;
	void setFlags(ServerMessageType serverMessageType);
	void setFlags(bool is_contact = false, bool incoming_contact_request = false, bool outgoing_contact_request = false) { this->is_contact = is_contact; this->incoming_contact_request = incoming_contact_request; this->outgoing_contact_request = outgoing_contact_request; }
	void setState(UserState newState);
private:
	virtual void mousePressEvent(QMouseEvent* e);
	UserState userState = UserState::unknown;
private slots:
	void removeContactClicked();
	void addContactClicked();
signals:
	void removeContact(const QString&);
	void addContact(const QString&);
	void userbutton_clicked(const QString&);
};

#endif // USERBUTTON_H

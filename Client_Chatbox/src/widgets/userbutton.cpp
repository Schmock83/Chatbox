#include "userbutton.h"

UserButton::UserButton(const QString& text, bool is_contact, bool incoming_contact_request, bool outgoing_contact_request, QWidget* parent)
	:QPushButton(text, parent), original_text(text), is_contact(is_contact), incoming_contact_request(incoming_contact_request), outgoing_contact_request(outgoing_contact_request)
{}
UserButton::UserButton(const QIcon& icon, const QString& text, bool is_contact, bool incoming_contact_request, bool outgoing_contact_request, QWidget* parent)
	: QPushButton(icon, text, parent), original_text(text), is_contact(is_contact), incoming_contact_request(incoming_contact_request), outgoing_contact_request(outgoing_contact_request)
{}

void UserButton::messageReceived()
{
	unreadMessageCount++;
	setText(tr("%1 (%2)").arg(original_text, QString::number(unreadMessageCount)));
}

void UserButton::mousePressEvent(QMouseEvent* e)
{
	QPushButton::mousePressEvent(e);
	if (e->button() == Qt::RightButton)
	{
		if (is_contact)
		{
			QMenu contextMenu(this);
			QAction action1("remove friend", this);
			QAction cancel_action("cancel", this);

			connect(&action1, SIGNAL(triggered()), this, SLOT(removeContactClicked()));
			connect(&cancel_action, SIGNAL(triggered()), &contextMenu, SLOT(close()));

			contextMenu.addAction(&action1);
			contextMenu.addAction(&cancel_action);
			contextMenu.exec(e->globalPos());
		}
		else if (incoming_contact_request)
		{
			QMenu contextMenu(this);
			QAction action1("accept friend request", this);
			QAction action2("decline friend request", this);
			QAction cancel_action("Cancel", this);

			connect(&action1, SIGNAL(triggered()), this, SLOT(addContactClicked()));
			connect(&action2, SIGNAL(triggered()), this, SLOT(removeContactClicked()));
			connect(&cancel_action, SIGNAL(triggered()), &contextMenu, SLOT(close()));

			contextMenu.addAction(&action1);
			contextMenu.addAction(&action2);
			contextMenu.addAction(&cancel_action);
			contextMenu.exec(e->globalPos());
		}
		else if (outgoing_contact_request)
		{
			QMenu contextMenu(this);
			QAction action1("cancel friend request", this);
			QAction cancel_action("cancel", this);

			connect(&action1, SIGNAL(triggered()), this, SLOT(removeContactClicked()));
			connect(&cancel_action, SIGNAL(triggered()), &contextMenu, SLOT(close()));

			contextMenu.addAction(&action1);
			contextMenu.addAction(&cancel_action);
			contextMenu.exec(e->globalPos());
		}
		else
		{
			QMenu contextMenu(this);
			QAction action1("send friend request", this);
			QAction cancel_action("cancel", this);

			connect(&action1, SIGNAL(triggered()), this, SLOT(addContactClicked()));
			connect(&cancel_action, SIGNAL(triggered()), &contextMenu, SLOT(close()));

			contextMenu.addAction(&action1);
			contextMenu.addAction(&cancel_action);
			contextMenu.exec(e->globalPos());
		}
	}
	else if (e->button() == Qt::LeftButton)
	{
		//reset unreadMessageCount and reset text
		unreadMessageCount = 0;
		setText(original_text);

		emit userbutton_clicked(text());
	}
}

void UserButton::setFlags(ServerMessageType serverMessageType)
{
	switch (serverMessageType)
	{
	case ServerMessageType::server_addContact:
		is_contact = true;
		outgoing_contact_request = incoming_contact_request = false;
		break;
	case ServerMessageType::server_addIncomingContactRequest:
		incoming_contact_request = true;
		outgoing_contact_request = is_contact = false;
		break;
	case ServerMessageType::server_addOutgoingContactRequest:
		outgoing_contact_request = true;
		incoming_contact_request = is_contact = false;
		break;
	}
}

void UserButton::removeContactClicked()
{
	emit removeContact(text());
}
void UserButton::addContactClicked()
{
	emit addContact(text());
}

void UserButton::setState(UserState newState)
{
	userState = newState;

	switch (userState)
	{
	case UserState::online:
		this->setIcon(QIcon(":/userIcons/imgs/online.png"));
		break;
	case UserState::offline:
		this->setIcon(QIcon(":/userIcons/imgs/offline.png"));
		break;
	case UserState::unknown:
		this->setIcon(QIcon());
	}
}
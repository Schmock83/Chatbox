#include "userbutton.h"

UserButton::UserButton(const QString& text, bool is_contact, QWidget* parent)
	:QPushButton(text, parent), is_contact(is_contact)
{}

void UserButton::mousePressEvent(QMouseEvent* e)
{
	QPushButton::mousePressEvent(e);
	if (e->button() == Qt::RightButton)
	{
		if (is_contact)
		{
			QMenu contextMenu_contact(this);
			QAction action1("Delete contact", this);
			QAction cancel_action("Cancel", this);

			connect(&action1, SIGNAL(triggered()), this, SLOT(deleteContactClicked()));
			connect(&cancel_action, SIGNAL(triggered()), &contextMenu_contact, SLOT(close()));

			contextMenu_contact.addAction(&action1);
			contextMenu_contact.addAction(&cancel_action);
			contextMenu_contact.exec(e->globalPos());
		}
		else
		{
			QMenu contextMenu_user(this);
			QAction action1("Add contact", this);
			QAction cancel_action("Cancel", this);

			connect(&action1, SIGNAL(triggered()), this, SLOT(addContactClicked()));
			connect(&cancel_action, SIGNAL(triggered()), &contextMenu_user, SLOT(close()));

			contextMenu_user.addAction(&action1);
			contextMenu_user.addAction(&cancel_action);
			contextMenu_user.exec(e->globalPos());
		}
	}
}

void UserButton::deleteContactClicked()
{
	emit deleteContact(text());
}
void UserButton::addContactClicked()
{
	emit addContact(text());
}
#ifndef USERBUTTON_H
#define USERBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QMouseEvent>
#include <QMenu>

#include <QDebug>

class UserButton : public QPushButton
{
	Q_OBJECT
public:
	UserButton(const QString& text, bool is_contact = false, QWidget* parent = nullptr);
private:
	bool is_contact;
	virtual void mousePressEvent(QMouseEvent* e);
private slots:
	void deleteContactClicked();
	void addContactClicked();
signals:
	void deleteContact(const QString&);
	void addContact(const QString&);
};

#endif // USERBUTTON_H

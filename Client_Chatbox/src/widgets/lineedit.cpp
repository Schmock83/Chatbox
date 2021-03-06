#include "lineedit.h"

LineEdit::LineEdit(QWidget* parent)
	:QLineEdit(parent)
{
	this->clearFocus();
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(textChanged_slot(QString)));
}

void LineEdit::focusInEvent(QFocusEvent* e)
{
	QLineEdit::focusInEvent(e);
	emit focussed();
}

void LineEdit::focusOutEvent(QFocusEvent* e)
{
	QLineEdit::focusOutEvent(e);
	emit unfocussed();
}

void LineEdit::textChanged_slot(QString newText)
{
	if (newText.isEmpty())
	{
		this->clearFocus();
		emit clearButtonPressed();
	}
}

bool LineEdit::event(QEvent* e)
{
	if (e->type() == QEvent::KeyPress)
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*>(e);
		if (key_event->key() == Qt::Key_Escape)
		{
			this->clear();
			this->clearFocus();
			emit escPressed();
		}
	}
	return QLineEdit::event(e);
}

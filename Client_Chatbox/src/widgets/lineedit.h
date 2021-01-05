#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QObject>
#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>

class LineEdit : public QLineEdit
{
	Q_OBJECT
public:
	LineEdit(QWidget* parent = nullptr);
protected:
	virtual void focusInEvent(QFocusEvent* e);
	virtual void focusOutEvent(QFocusEvent* e);
	virtual bool event(QEvent* e);
private slots:
	void textChanged_slot(const QString&);

signals:
	void focussed();
	void unfocussed();
	void clearButtonPressed();
	void escPressed();
};

#endif // LINEEDIT_H

#ifndef CHATBROWSER_H
#define CHATBROWSER_H

//Overloaded class of QTextBrowser - representing a chat
//messages will be sorted by dateTime automatically
//also give the user the possibility of requesting older messages, by pressing a button at the top of the chat

#include <QObject>
#include <QTextBrowser>
#include <QScrollBar>
#include <QDateTime>
#include <QVector>
#include <QPair>
#include <QPushButton>

class ChatBrowser : public QTextBrowser
{
	Q_OBJECT
public:
	ChatBrowser();
	void appendToChatHistory(QDateTime datetime, QString message);
	bool CursorAtBottom();
	bool CursorAtTop();
	void setCursorToBottom();
	void displayMessages();
	void insert(const QPair<QDateTime, QString>& pair);
	void insertDateLabel(const QDateTime& datetime);
	void refreshButton();
	void noEarlierMessagesAvailable();
protected:
	void resizeEvent(QResizeEvent* event);
private slots:
	void erlierMessagesBtnPressed();
	void sliderValueChanged();
signals:
	void queryEarlierMessages(QDateTime);
private:
	QVector<QPair<QDateTime, QString>> messages;
	QVector<QDate> date_messages;
	QPushButton* loadMessagesBtn;
};

#endif // CHATBROWSER_H

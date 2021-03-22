#ifndef CHATBROWSER_H
#define CHATBROWSER_H

//Overloaded class of QTextBrowser - representing a chat
//messages will be sorted by dateTime automatically

#include <QObject>
#include <QTextBrowser>
#include <QScrollBar>
#include <QDateTime>
#include <QVector>
#include <QPair>
#include <QLabel>
#include <QMovie>

#include "../core_includes/messages/messagewrapper.h"

class ChatBrowser : public QTextBrowser
{
	Q_OBJECT
public:
	ChatBrowser(QString chat_user_name);
	void appendToChatHistory(QDateTime datetime, QString message);
    void appendToChatHistory(Client_Chat_Message* chat_Message);
	bool CursorAtBottom();
	bool CursorAtTop();
	void setCursorToBottom();
	void displayMessages();
	void insert(const QPair<QDateTime, QString>& pair);
	void insertDateLabel(const QDateTime& datetime);
    void repositionLoadingLabel();
	void noEarlierMessagesAvailable();
protected:
	void resizeEvent(QResizeEvent* event);
private slots:
	void sliderValueChanged();
signals:
	void requestOlderMessages(QString, QDateTime);
private:
	QVector<QPair<QDateTime, QString>> messages;
	QString chat_user_name;
	QVector<QDate> date_messages;
	QLabel* loading_label;
	QMovie* loading_animation;
    void disableLoadingLabel();
    void showNoEarlierMessagesAvailableInChat();
};

#endif // CHATBROWSER_H

#include "chatbrowser.h"

ChatBrowser::ChatBrowser(QString chat_user_name)
	:chat_user_name(chat_user_name)
{
	//show a 'No recorded Messages found' on an empty chat -> will be overwritten when first insertion happens
	setHtml(tr("<p style=\"text-align:center; font-size: 14px;\">"
		"<span style=\"padding: 10px; border-radius: 10px;\">"
		"This chat is empty ..."
		"</span></p>"));

	//initiate loading animation
	loading_animation = new QMovie(":/loadingGifs/imgs/loading3.gif");
	loading_label = new QLabel(this);
	loading_label->setAlignment(Qt::AlignCenter);
	loading_label->setMovie(loading_animation);
	loading_label->hide();

	//refresh label to new center
    repositionLoadingLabel();

	//connect verticalSlider-valueChanged(int) slot with sliderValueChanged(int) --> to display the loading animation whenever the user is at the top of the chatHistory
	connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
}

void ChatBrowser::appendToChatHistory(Client_Chat_Message* chat_Message)
{
    if (chat_Message->getReceiver() == chat_user_name)
	{
        appendToChatHistory(chat_Message->getDateTime(), tr(
			"<p style=\"margin-bottom:0em; margin-top:0em; text-align:left; width: 50%; font-size: 14px;\">%1"
			"<div style=\"font-size: 18px; margin-bottom: 1em;\">%2</div>"
			"</p>"
        ).arg(chat_Message->getDateTime().toString("hh:mm:ss"), chat_Message->getContent()));
	}
	else
	{
        appendToChatHistory(chat_Message->getDateTime(), tr(
			"<p style=\"margin-bottom:0em; margin-top:0em; text-align:right; width: 50%; font-size: 14px;\">%1"
			"<div style=\"font-size: 18px; margin-bottom: 1em;\">%2</div>"
            "</p>").arg(chat_Message->getDateTime().toString("hh:mm:ss"), chat_Message->getContent()));
	}
}

void ChatBrowser::appendToChatHistory(QDateTime datetime, QString message)
{
	disconnect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

	int old_slider_pos = verticalScrollBar()->value();
	//stop loading animation
	loading_animation->stop();
	loading_label->hide();

	QPair<QDateTime, QString> pair = QPair<QDateTime, QString>(datetime, message);

	if (messages.contains(pair)) //msg alredy in messages -> do nothing
		return;

	//check if date is after

	//if user was at the bottom of the chat -> set it to hte new bottom at the end

	//insert message -> for display later
	insert(pair);
	//insert dateLabel into chatBrowser
	insertDateLabel(datetime);
	//update chatBrowser
	displayMessages();

	if (verticalScrollBar()->minimum() == verticalScrollBar()->maximum())
		setCursorToBottom();
	else
		verticalScrollBar()->setValue(old_slider_pos);

	connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
}

//update chatBrowser (messages)
void ChatBrowser::displayMessages()
{
	QString html_output;
    for (auto msg : messages) {
		html_output += msg.second;
	}

	setHtml(html_output);
	ensureCursorVisible();
}

void ChatBrowser::setCursorToBottom()
{
	disconnect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	moveCursor(QTextCursor::MoveOperation::End);
	connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
}

bool compareFunc(const QPair<QDateTime, QString> pair1, const QPair<QDateTime, QString> pair2) {
	return pair1.first < pair2.first;
}

void ChatBrowser::insert(const QPair<QDateTime, QString>& pair)
{
	//sorted insertion
	messages.insert(
		std::upper_bound(messages.begin(), messages.end(), pair, compareFunc),
		pair
	);
}

void ChatBrowser::insertDateLabel(const QDateTime& datetime)
{
	//for inserting a new date-label so every message with that date can be sorted correctly
	QDateTime dateTimeLabel = QDateTime::fromString(tr("%1 00:00:00").arg(datetime.date().toString("dd.MM.yyyy")), "dd.MM.yyyy hh:mm:ss");

	//insert dateLabel into chatBrowser, so user knows that all messages below are from that date
	QString msg = tr("<p style=\"text-align:center; font-size: 14px;\">"
		"<span style=\"padding: 10px; border-radius: 10px;\">"
		"%1"
		"</span></p>").arg(datetime.date().toString("dd.MM.yyyy"));
	QPair<QDateTime, QString> datePair = QPair<QDateTime, QString>(dateTimeLabel, msg);

	if (!messages.contains(datePair))
	{
		insert(QPair<QDateTime, QString>(dateTimeLabel, msg));
	}
}

void ChatBrowser::disableLoadingLabel()
{
    //hide and disable loading animation
    loading_animation->stop();
    loading_label->hide();
}

void ChatBrowser::showNoEarlierMessagesAvailableInChat()
{
    QDateTime dateTime;
    dateTime = QDateTime::fromString(tr("%1 00:00:00").arg(messages[0].first.toString("yyy-MM-dd")), "yyyy-MM-dd hh:mm:ss");
    QString message = tr("<p style=\"text-align:center; font-size: 14px;\">"
        "<span style=\"padding: 10px; border-radius: 10px;\">"
        "No earlier messages are available"
        "</span></p>");

    appendToChatHistory(dateTime, message);
}

void ChatBrowser::noEarlierMessagesAvailable()
{
    //disconnect verticalSlider so that loadingAnimation isnt displayed
	disconnect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

    disableLoadingLabel();

    //print out message "no earlier messages available"
    showNoEarlierMessagesAvailableInChat();
}

void ChatBrowser::sliderValueChanged()
{
    if (CursorAtTop())  //only show loading animation, when Slider at top
	{
        if (loading_animation->state() == QMovie::Running)  //when user reached the top - show loadingAnimation if its running
		{
			loading_label->show();
		}
        else if (!messages.empty() && loading_animation->state() != QMovie::Running) {  //if the loadingAnimation isnt running -> start it
			emit requestOlderMessages(chat_user_name, messages[0].first.addDays(-1));
			loading_animation->start();
			loading_label->show();
		}
	}
	else
	{
		loading_label->hide();
	}
}

void ChatBrowser::resizeEvent(QResizeEvent* event)
{
	QTextEdit::resizeEvent(event);

    repositionLoadingLabel();
}

void ChatBrowser::repositionLoadingLabel()
{
	loading_label->setGeometry((this->width() / 2) - 70, 10, 120, 80);
}

bool ChatBrowser::CursorAtBottom()
{
	return verticalScrollBar()->value() == verticalScrollBar()->maximum();
}

bool ChatBrowser::CursorAtTop()
{
	return verticalScrollBar()->value() == verticalScrollBar()->minimum();
}

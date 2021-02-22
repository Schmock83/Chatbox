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
	refreshLoadingLabel();

	//connect verticalSlider-valueChanged(int) slot with sliderValueChanged(int) --> to display the loading animation whenever the user is at the top of the chatHistory
	connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
}

void ChatBrowser::appendToChatHistory(const Message message)
{
	if (message.getReceiver() == chat_user_name)
	{
		appendToChatHistory(message.getDateTime(), tr(
			"<p style=\"margin-bottom:0em; margin-top:0em; text-align:left; width: 50%; font-size: 14px;\">%1"
			"<div style=\"font-size: 18px; margin-bottom: 1em;\">%2</div>"
			"</p>"
		).arg(message.getDateTime().toString("hh:mm:ss"), message.getContent()));
	}
	else
	{
		appendToChatHistory(message.getDateTime(), tr(
			"<p style=\"margin-bottom:0em; margin-top:0em; text-align:right; width: 50%; font-size: 14px;\">%1"
			"<div style=\"font-size: 18px; margin-bottom: 1em;\">%2</div>"
			"</p>").arg(message.getDateTime().toString("hh:mm:ss"), message.getContent()));
	}
}

void ChatBrowser::appendToChatHistory(QDateTime datetime, QString message)
{
	//stop loading animation
	loading_animation->stop();
	loading_label->hide();

	QPair<QDateTime, QString> pair = QPair<QDateTime, QString>(datetime, message);

	if (messages.contains(pair)) //msg alredy in messages -> do nothing
		return;

	//check if date is after

	//if user was at the bottom of the chat -> set it to hte new bottom at the end

	//store if Cursor was at Bottom, because when we insert it will be repositioned
	bool flag = CursorAtBottom();
	//insert message -> for display later
	insert(pair);
	//insert dateLabel into chatBrowser
	insertDateLabel(datetime);
	//update chatBrowser
	displayMessages();

	//when cursor was at the bottom before insertion -> re-set it to bottom
	if (flag)
		setCursorToBottom();
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
	moveCursor(QTextCursor::MoveOperation::End);
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

void ChatBrowser::noEarlierMessagesAvailable()
{
	//disconnect verticalSlider-valueChanged(int) slot with sliderValueChanged(int) --> to display the loadMessagesBtn whenever the user is at the top of the chatHistory
	disconnect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

	//hide and disable loading animation
	loading_animation->stop();
	loading_label->hide();

	//inform user in chatbrowser, that no erlier message are available
	QDateTime dateTime;
	if (messages.size() > 0)
	{
		dateTime = QDateTime::fromString(tr("%1 00:00:00").arg(messages[0].first.toString("yyy-MM-dd")), "yyyy-MM-dd hh:mm:ss");
		QString message = tr("<p style=\"text-align:center; font-size: 14px;\">"
			"<span style=\"padding: 10px; border-radius: 10px;\">"
			"No earlier messages are available"
			"</span></p>");

		//append message to chat
		appendToChatHistory(dateTime, message);
	}
}

void ChatBrowser::sliderValueChanged()
{
	if (CursorAtTop())    //only show loading animation, when Slider at top
	{
		if (loading_animation->state() == QMovie::Running)
		{
			loading_label->show();
		}
		else if (!messages.empty() && loading_animation->state() != QMovie::Running) {
			emit queryEarlierMessages(chat_user_name, messages[0].first.addDays(-1));
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

	//re-size label
	refreshLoadingLabel();
}

void ChatBrowser::refreshLoadingLabel()
{
	//re-position label
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
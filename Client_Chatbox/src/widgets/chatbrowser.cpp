#include "chatbrowser.h"

ChatBrowser::ChatBrowser()
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

	//refresh button to new center
	refreshLoadingLabel();

	//connect verticalSlider-valueChanged(int) slot with sliderValueChanged(int) --> to display the loadMessagesBtn whenever the user is at the top of the chatHistory
	connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
}

void ChatBrowser::appendToChatHistory(QDateTime datetime, QString message)
{
	//stop loading animation
	loading_animation->stop();
	loading_label->hide();

	QPair<QDateTime, QString> pair = QPair<QDateTime, QString>(datetime, message);

	if (messages.contains(pair)) //msg alredy in messages -> do nothing
		return;

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
	//clear current chatBrowser
	clear();

	//display all the msgs
	for (auto msg : messages)
		setHtml(toHtml().append(msg.second));
}

void ChatBrowser::setCursorToBottom()
{
	verticalScrollBar()->setValue(verticalScrollBar()->maximum());
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
	if (CursorAtTop())    //only show loadMessgesBtn, when Slider at top
	{
		if (loading_animation->state() == QMovie::Running)
		{
			loading_label->show();
		}
		else if (!messages.empty() && loading_animation->state() != QMovie::Running) {
			//emit queryEarlierMessages(messages[0].first.addDays(-1));
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

	//re-size button
	refreshLoadingLabel();
}

void ChatBrowser::refreshLoadingLabel()
{
	//re-position label
	loading_label->setGeometry((this->width() / 2) - 70, 10, 100, 80);
}

bool ChatBrowser::CursorAtBottom()
{
	return verticalScrollBar()->value() == verticalScrollBar()->maximum();
}

bool ChatBrowser::CursorAtTop()
{
	return verticalScrollBar()->value() == verticalScrollBar()->minimum();
}
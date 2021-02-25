#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, client(new Chatbox_Client(this))
{
	ui->setupUi(this);

	setUpSignalSlotConnections();

	initializeUI();

	//emit establishSocketConnection_signal();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setUpSignalSlotConnections()
{
	//LineEdit (for searching users)
	connect(ui->search_line_edit, SIGNAL(focussed()), this, SLOT(search_line_edit_focussed()));
	connect(ui->search_line_edit, SIGNAL(clearButtonPressed()), this, SLOT(search_line_edit_unfocussed()));
	connect(ui->search_line_edit, SIGNAL(escPressed()), this, SLOT(search_line_edit_unfocussed()));
	connect(ui->search_line_edit, SIGNAL(returnPressed()), this, SLOT(search_line_edit_returnPressed()));

	//buttons
	connect(ui->login_Button, SIGNAL(clicked()), this, SLOT(login_button_pressed()));
	connect(ui->login_UsernameEdit, SIGNAL(returnPressed()), this, SLOT(login_button_pressed()));
	connect(ui->login_PasswordEdit, SIGNAL(returnPressed()), this, SLOT(login_button_pressed()));

	connect(ui->registration_Button, SIGNAL(clicked()), this, SLOT(registration_button_pressed()));
	connect(ui->registration_UsernameEdit, SIGNAL(returnPressed()), this, SLOT(registration_button_pressed()));
	connect(ui->registration_PasswordEdit, SIGNAL(returnPressed()), this, SLOT(registration_button_pressed()));

	connect(this, SIGNAL(establishSocketConnection_signal()), client, SLOT(establishSocketConnection()));

	//UI
	connect(client, SIGNAL(clearLoadingErrorLabel()), this, SLOT(clearLoadingErrorLabel()));
	connect(client, SIGNAL(setScene(UI::Scene)), this, SLOT(setScene(UI::Scene)));
	connect(this, SIGNAL(sceneChanged(UI::Scene)), client, SLOT(sceneChanged(UI::Scene)));
	connect(client, SIGNAL(setLoadingError(QString)), this, SLOT(setLoadingError(QString)));
	connect(client, SIGNAL(setLoadingStatus(QString)), this, SLOT(setLoadingStatus(QString)));
	connect(client, SIGNAL(startLoadingTimer()), this, SLOT(startLoadingTimer()));
	connect(client, SIGNAL(startLoadingPageAnimation()), this, SLOT(startLoadingPageAnimation()));
	connect(client, SIGNAL(stopLoadingPageAnimation()), this, SLOT(stopLoadingPageAnimation()));
	connect(client, SIGNAL(startWelcomePageAnimation()), this, SLOT(startWelcomePageAnimation()));
	connect(client, SIGNAL(stopWelcomePageAnimation()), this, SLOT(stopWelcomePageAnimation()));
	connect(client, SIGNAL(enableButtons()), this, SLOT(enableButtons()));
	connect(client, SIGNAL(disableButtons()), this, SLOT(disableButtons()));
	connect(client, SIGNAL(clearRegistrationPasswordEdit()), this, SLOT(clearRegistrationPasswordEdit()));
	connect(client, SIGNAL(clearRegistrationStatusLabel()), this, SLOT(clearRegistrationStatusLabel()));
	connect(client, SIGNAL(clearLoginPasswordEdit()), this, SLOT(clearLoginPasswordEdit()));
	connect(client, SIGNAL(clearLoginStatusLabel()), this, SLOT(clearLoginStatusLabel()));
	connect(client, SIGNAL(setRegistrationStatus(QString)), this, SLOT(setRegistrationStatus(QString)));
	connect(client, SIGNAL(setLoginStatus(QString)), this, SLOT(setLoginStatus(QString)));
	connect(client, SIGNAL(setLoginError(QString)), this, SLOT(setLoginError(QString)));
	connect(client, SIGNAL(setRegistrationError(QString)), this, SLOT(setRegistrationError(QString)));

	connect(this, SIGNAL(searchUserSignal(const QString&)), client, SLOT(searchUser(const QString&)));
	connect(client, SIGNAL(searchUsersSignal(QList<QString>)), this, SLOT(addSearchedUsers(QList<QString>)));
	connect(this, SIGNAL(addContactSignal(const QString&)), client, SLOT(addContact(const QString&)));
	connect(client, SIGNAL(addContactSignal(const QString&)), this, SLOT(addContact(const QString&)));
	connect(client, SIGNAL(removeContactSignal(const QString&)), this, SLOT(removeContact(const QString&)));
	connect(client, SIGNAL(addContactRequestSignal(const QString&, ServerMessageType)), this, SLOT(addContactRequest(const QString&, ServerMessageType)));
	connect(client, SIGNAL(removeContactRequestSignal(const QString&)), this, SLOT(removeContactRequest(const QString&)));
	connect(client, SIGNAL(clearUI()), this, SLOT(clearUI()));
	connect(client, SIGNAL(userStateChanged(QPair<QString, UserState>)), this, SLOT(userStateChanged(QPair<QString, UserState>)));
	connect(client, SIGNAL(chatMessageReceived(const Message&)), this, SLOT(chatMessageReceived(const Message&)));
	connect(client, SIGNAL(oldChatMessageReceived(const Message&, bool)), this, SLOT(oldChatMessageReceived(const Message&, bool)));
	connect(client, SIGNAL(noOlderMessagesAvailable(QString)), this, SLOT(noOlderMessagesAvailable(QString)));
	connect(client, SIGNAL(showError(QString)), this, SLOT(showPopupInformationBox(QString)));
	connect(client, SIGNAL(updateUserName(const QString&)), this, SLOT(setUserNameLabel(const QString&)));
}
#include <QPixmap>
void MainWindow::initializeUI()
{
	ui->chats_grid_layout->setAlignment(Qt::AlignTop);
	ui->contacts_layout->setAlignment(Qt::AlignTop);
	ui->user_search_layout->setAlignment(Qt::AlignTop);

	//initiate loading animation
	loadingAnimation = new QMovie(":/loadingGifs/imgs/loading3.gif");
	loadingAnimation->start();
	ui->loadingPage_loading_label->setMovie(loadingAnimation);
	stopLoadingPageAnimation();

	//loading animation for welcomeScene
	ui->welcomePage_loading_label->setMovie(loadingAnimation);
	stopWelcomePageAnimation();

	UI::setUpDarkTheme();

	ui->chat_contacts_stackedWidget->setCurrentIndex(UI::ChatContactPage::contactPage);

	setScene(UI::Scene::mainScene);

	updateContactList();

	ui->user_state_label->setPixmap(QPixmap(":/userIcons/imgs/online.png").scaled(40, 30, Qt::AspectRatioMode::KeepAspectRatio));
	ui->user_state_label->setToolTip("Go offline");
}

void MainWindow::clearUI()
{
	deleteWidgetsFromLayout(ui->user_search_layout, true);

	//clear contacts
	deleteWidgetsFromLayout(ui->contacts_layout, true);

	contacts.clear();

	//clear contact-requests
	contact_requests.clear();

	deleteWidgetsFromLayout(ui->chats_grid_layout, true);

	//clear chat-buttons
	chatButtons.clear();
}

void MainWindow::on_chat_button_clicked()
{
	ui->chat_contacts_stackedWidget->setCurrentIndex(UI::ChatContactPage::chatPage);
}

void MainWindow::on_contacts_button_clicked()
{
	ui->chat_contacts_stackedWidget->setCurrentIndex(UI::ChatContactPage::contactPage);
}

void MainWindow::search_line_edit_focussed()
{
	if (ui->chat_contacts_stackedWidget->currentIndex() != UI::ChatContactPage::user_search_page)
		previous_left_page = (UI::ChatContactPage)ui->chat_contacts_stackedWidget->currentIndex();
	ui->chat_contacts_stackedWidget->setCurrentIndex(UI::ChatContactPage::user_search_page);
}
void MainWindow::search_line_edit_unfocussed()
{
	ui->chat_contacts_stackedWidget->setCurrentIndex(previous_left_page);
}
void MainWindow::search_line_edit_returnPressed()
{
	ui->search_line_edit->clearFocus();

	//delete old results
	deleteWidgetsFromLayout(ui->user_search_layout->layout());

	//show loading animation
	QLabel* loadingLabel = new QLabel("Hello");
	loadingLabel->setAlignment(Qt::AlignCenter);
	loadingLabel->setMovie(loadingAnimation);
	loadingAnimation->start();
	ui->user_search_layout->addWidget(loadingLabel);

	emit searchUserSignal(ui->search_line_edit->text());
}

void MainWindow::userStateChanged(QPair<QString, UserState> pair)
{
	//get userbutton associated with username
	auto it = contacts[pair.first[0]].find(pair.first);
	if (it != contacts[pair.first[0]].end())
	{
		(*it)->setState(pair.second);
	}

	//show popup
	if (pair.second == UserState::online)
	{
		showPopupInformationBox(QString("%1 went online").arg(pair.first));
	}
	else if (pair.second == UserState::offline)
	{
		showPopupInformationBox(QString("%1 went offline").arg(pair.first));
	}
}

void MainWindow::addSearchedUsers(QList<QString> searchedUsers)
{
	//delete everything from the searchUser-layout - including the loading label
	deleteWidgetsFromLayout(ui->user_search_layout->layout());

	UserButton* user_btn;
	for (auto searchedUser : searchedUsers)
	{
		user_btn = new UserButton(searchedUser, isContact(searchedUser), hasIncomingContactRequest(searchedUser), hasOutgoingContactRequest(searchedUser));
		connect(user_btn, SIGNAL(addContact(const QString&)), client, SLOT(addContact(const QString&)));
		connect(user_btn, SIGNAL(removeContact(const QString&)), client, SLOT(removeContact(const QString&)));
		connect(user_btn, SIGNAL(userbutton_clicked(const QString&)), this, SLOT(showChatWindow(const QString&)));
		ui->user_search_layout->addWidget(user_btn);
	}
}

void MainWindow::startLoadingTimer()
{
	//wait 6 seconds and then call establishSocketConnection again...
	cooldown_timer = cooldown_secs;

	//timer for every second to update text
	updateLoadingScreenLabel();
}

void MainWindow::updateLoadingScreenLabel()
{
	setLoadingStatus(tr("Trying again in %2 seconds...").arg(QString::number(cooldown_timer)));
	cooldown_timer--;
	if (cooldown_timer >= 0)
		QTimer::singleShot(1000, this, SLOT(updateLoadingScreenLabel()));
	else
		emit client->establishSocketConnection();
}

void MainWindow::setScene(UI::Scene scene)
{
	ui->sceneWidget->setCurrentIndex(scene);
	emit sceneChanged(scene);

	if (scene == UI::Scene::mainScene) {
		if (!information_box)
			information_box = new Fading_Information_Box(this);
		information_box->setWindowFlag(Qt::WindowStaysOnTopHint);
		information_box->setGeometry(this->width() - information_box->width(), this->height() - information_box->height(), information_box->width(), information_box->height());
	}
	else if (information_box) {//hide on other scenes
		information_box->fade_out();
	}
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	if (ui->sceneWidget->currentIndex() == UI::Scene::mainScene && information_box)
		information_box->setGeometry(this->width() - information_box->width(), this->height() - information_box->height(), information_box->width(), information_box->height());
}

//puts newTopButton to the front of chatButtons-list (adds or re-adds if already in list)
void MainWindow::addTopChatButton(UserButton* newTopButton)
{
	//remove if exists
	chatButtons.remove(newTopButton);

	//re-add to front
	chatButtons.push_front(newTopButton);

	updateChatList();
}

void MainWindow::showChatWindow(const QString& user_name)
{
	int index = getChatWindowIndex(user_name);

	//no chatWindow for that user exists -> create one
	if (index == -1)
	{
		index = buildChatWindow(user_name);
	}

	//actually show the chatWindow
	ui->stacked_chat_browsers->setCurrentIndex(index);

	UserButton* chat_btn = getChatButton(user_name);
	if (chat_btn != nullptr)
	{
		//reset text
		chat_btn->setText(chat_btn->text());
	}
}

int MainWindow::getChatWindowIndex(const QString& user_name)
{
	if (chatWindows.contains(user_name))
		return ui->stacked_chat_browsers->indexOf(chatWindows[user_name]);

	return -1;
}

int MainWindow::buildChatWindow(const QString& user_name)
{
	//check if it already exists
	if (chatWindows.contains(user_name))
		return -1;

	//otherwise create widgets and add them to the chatStackedWidget
	QWidget* chatWindow = new QWidget;
	QVBoxLayout* vboxLayout = new QVBoxLayout;
	ChatBrowser* chat = new ChatBrowser(user_name);
	QPushButton* sendButton = new QPushButton("Send");
	QLineEdit* lineEdit = new QLineEdit;

	connect(sendButton, SIGNAL(clicked()), this, SLOT(sendMessage()));
	connect(chat, SIGNAL(queryEarlierMessages(QString, QDateTime)), this, SLOT(requestOlderMessages(QString, QDateTime)));
	connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(sendMessage()));

	vboxLayout->addWidget(new QLabel(tr("Chat with ").append(user_name)));
	vboxLayout->addWidget(chat);
	vboxLayout->addWidget(lineEdit);
	vboxLayout->addWidget(sendButton);

	chatWindow->setLayout(vboxLayout);

	//add the widget to the stackedChatWidget
	int index = ui->stacked_chat_browsers->addWidget(chatWindow);

	chatWindows[user_name] = chatWindow;

	//create chat-button
	if (getChatButton(user_name) == nullptr)
		addChatButton(user_name);

	return index;
}

void MainWindow::requestOlderMessages(QString chat_user_name, QDateTime dateTime)
{
	//delegate to client
	Message request = Message::createClientRequstMessage(dateTime, ClientRequestType::olderMessages, chat_user_name);
	client->requestOlderMessages(request);
}

//when send button was clicked -> send message
void MainWindow::sendMessage()
{
	//get the current line-edit field containing the message
	QWidget* currentChatWindow = ui->stacked_chat_browsers->currentWidget();
	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(currentChatWindow->layout()->itemAt(2)->widget());

	QString receiver = chatWindows.key(currentChatWindow);
	QString message = lineEdit->text();

	if (message.isEmpty())
	{
		//empty message
		return;
	}

	//build message
	Message msg = Message::createChatMessage(receiver, QDateTime::currentDateTime(), message);

	//send message
	client->queue_message(msg);
	client->deliver_queued_messages();

	//append to the chat
	appendToChatHistory(receiver, msg);

	lineEdit->clear();
}

void MainWindow::chatMessageReceived(const Message& message)
{
	appendToChatHistory(message.getSender(), message);
}

void MainWindow::oldChatMessageReceived(const Message& message, bool receiver)
{
	if (receiver)
	{
		chatMessageReceived(message);
	}
	else
	{
		appendToChatHistory(message.getReceiver(), message);
	}
}

void MainWindow::noOlderMessagesAvailable(QString username)
{
	//get the chatbrowser of the chatwindow associated with username

	int index = getChatWindowIndex(username);

	//no chatWindow for that user exists -> nothing to do
	if (index == -1)
	{
		return;
	}

	ChatBrowser* chatBrowser = getChatForIndex(index);
	if (chatBrowser != nullptr)
	{
		chatBrowser->noEarlierMessagesAvailable();
	}
}

void MainWindow::appendToChatHistory(QString chat_user_name, const Message message)
{
	int index = getChatWindowIndex(chat_user_name);

	//no chatWindow for that user exists -> create one
	if (index == -1)
	{
		index = buildChatWindow(chat_user_name);
	}

	//add chatButton to the top
	//1.check if chatButton exists
	UserButton* chatButton = getChatButton(chat_user_name);
	if (chatButton == nullptr)
	{
		chatButton = addChatButton(chat_user_name);
	}

	addTopChatButton(chatButton);
	ChatBrowser* chatBrowser = getChatForIndex(index);
	if (chatBrowser != nullptr)
	{
		chatBrowser->appendToChatHistory(message);

		//if chat is not currently shown and its a 'new' chatMessage -> show '+1 message' on chatButton
		if (message.getMessageType() == MessageType::chatMessage && ui->stacked_chat_browsers->currentIndex() != index)
		{
			chatButton->messageReceived();
		}
	}
}

UserButton* MainWindow::addChatButton(const QString& chat_user_name)
{
	if (getChatButton(chat_user_name) == nullptr)
	{
		UserButton* chat_btn = new UserButton(chat_user_name, isContact(chat_user_name), hasIncomingContactRequest(chat_user_name), hasOutgoingContactRequest(chat_user_name));
		connect(chat_btn, SIGNAL(addContact(const QString&)), client, SLOT(addContact(const QString&)));
		connect(chat_btn, SIGNAL(removeContact(const QString&)), client, SLOT(removeContact(const QString&)));
		connect(chat_btn, SIGNAL(userbutton_clicked(const QString&)), this, SLOT(showChatWindow(const QString&)));

		addTopChatButton(chat_btn);

		return chat_btn;
	}

	return nullptr;
}

UserButton* MainWindow::getChatButton(const QString& chat_name)
{
	for (auto chatButton : chatButtons)
		if (chatButton->text() == chat_name)
			return chatButton;

	return nullptr;
}

ChatBrowser* MainWindow::getChatForIndex(int index)
{
	return qobject_cast<ChatBrowser*>(ui->stacked_chat_browsers->widget(index)->layout()->itemAt(1)->widget());
}

bool MainWindow::isContact(const QString& user_name)
{
	return contacts[user_name[0]].contains(user_name);
}

bool MainWindow::hasIncomingContactRequest(const QString& user_name)
{
	if (!contact_requests.contains(user_name))
		return false;
	UserButton* user_btn = contact_requests[user_name];
	if (user_btn != nullptr)
		return user_btn->incoming_contact_request;
	return false;
}

bool MainWindow::hasOutgoingContactRequest(const QString& user_name)
{
	if (!contact_requests.contains(user_name))
		return false;
	UserButton* user_btn = contact_requests[user_name];
	if (user_btn != nullptr)
		return user_btn->outgoing_contact_request;
	return false;
}

void MainWindow::setUserNameLabel(const QString& str)
{
	//shorten username
	if (str.size() > 10)
	{
		QStringRef shorted_user_name(&str, 0, 10);
		ui->user_name_label->setText(shorted_user_name.toString().append("..."));
	}
	else
		ui->user_name_label->setText(str);
}

void MainWindow::showPopupInformationBox(const QString& str)
{
	if (information_box && ui->sceneWidget->currentIndex() == UI::Scene::mainScene)
	{
		information_box->showInfo(str);
	}
}

void MainWindow::updateChatList()
{
	//delete all buttons from layout
	deleteWidgetsFromLayout(ui->chats_grid_layout->layout(), false);

	//re-add them
	for (const auto& button : chatButtons)
		ui->chats_grid_layout->addWidget(button);
}

void MainWindow::addContact(const QString& contact)
{
	//remove contact from friend-request
	contact_requests.remove(contact);

	updateSearchedUser(contact, true);
	updateChatButton(contact, true);

	UserButton* contactButton = new UserButton(contact, true);
	contactButton->setState(UserState::offline);
	connect(contactButton, SIGNAL(addContact(const QString&)), client, SLOT(addContact(const QString&)));
	connect(contactButton, SIGNAL(removeContact(const QString&)), client, SLOT(removeContact(const QString&)));
	connect(contactButton, SIGNAL(userbutton_clicked(const QString&)), this, SLOT(showChatWindow(const QString&)));
	contacts[contact[0]].insert(contact, contactButton);

	updateContactList();
}

void MainWindow::removeContact(const QString& contact)
{
	contacts[contact[0]].remove(contact);

	updateSearchedUser(contact, false);
	updateChatButton(contact, false);

	updateContactList();
}

void MainWindow::addContactRequest(const QString& contact, ServerMessageType serverMessageType)
{
	UserButton* contact_request_button = new UserButton(contact);
	contact_request_button->setFlags(serverMessageType);
	connect(contact_request_button, SIGNAL(addContact(const QString&)), client, SLOT(addContact(const QString&)));
	connect(contact_request_button, SIGNAL(removeContact(const QString&)), client, SLOT(removeContact(const QString&)));
	connect(contact_request_button, SIGNAL(userbutton_clicked(const QString&)), this, SLOT(showChatWindow(const QString&)));
	contact_requests.insert(contact, contact_request_button);

	updateSearchedUser(contact, serverMessageType);

	updateContactList();
}

void MainWindow::removeContactRequest(const QString& contact)
{
	contact_requests.remove(contact);

	updateSearchedUser(contact, false);
	updateChatButton(contact, false);

	updateContactList();
}

UserButton* MainWindow::getContactRequestButton(const QString& user_name)
{
	UserButton* user_btn;
	for (int i = 0; i < ui->user_search_layout->count(); i++)
	{
		QWidget* widget = ui->user_search_layout->itemAt(i)->widget();
		user_btn = dynamic_cast<UserButton*>(widget);
		if (user_btn != nullptr && user_btn->text() == user_name)
		{
			return user_btn;
		}
	}

	return nullptr;
}

//for updating a searchedUser that was added/removed from contacts (so that context-menu shows the right tooltip + signals match)
void MainWindow::updateSearchedUser(const QString& user_name, bool is_contact, bool incoming_contact_request, bool outgoing_contact_request)
{
	UserButton* user_btn = getContactRequestButton(user_name);
	if (user_btn != nullptr)
		user_btn->setFlags(is_contact, incoming_contact_request, outgoing_contact_request);
}

void MainWindow::updateSearchedUser(const QString& user_name, ServerMessageType serverMessageType)
{
	UserButton* user_btn = getContactRequestButton(user_name);
	if (user_btn != nullptr)
		user_btn->setFlags(serverMessageType);
}

//for updating a searchedUser that was added/removed from contacts (so that context-menu shows the right tooltip + signals match)
void MainWindow::updateChatButton(const QString& user_name, bool is_contact, bool incoming_contact_request, bool outgoing_contact_request)
{
	UserButton* chat_btn = getChatButton(user_name);
	if (chat_btn != nullptr)
		chat_btn->setFlags(is_contact, incoming_contact_request, outgoing_contact_request);
}

void MainWindow::updateChatButton(const QString& user_name, ServerMessageType serverMessageType)
{
	UserButton* chat_btn = getChatButton(user_name);
	if (chat_btn != nullptr)
		chat_btn->setFlags(serverMessageType);
}

void MainWindow::updateContactList()
{
	//clear all widgets in the contact-list
	deleteWidgetsFromLayout(ui->contacts_layout->layout(), false);

	//re-add all contact-requests
	if (!contact_requests.isEmpty())
	{
		QLabel* headerLabel = new QLabel("Contact requests");
		headerLabel->setAlignment(Qt::AlignCenter);
		ui->contacts_layout->addWidget(headerLabel);

		for (auto it : contact_requests)
		{
			ui->contacts_layout->addWidget(it);
		}
	}

	QLabel* headerLabel = new QLabel("Contacts");
	headerLabel->setAlignment(Qt::AlignCenter);
	ui->contacts_layout->addWidget(headerLabel);

	//re-add all contacts + headerLabels, indicating start character
	QMapIterator<QChar, QMap<QString, UserButton*>> it(contacts);
	while (it.hasNext()) {
		it.next();
		if (it.value().isEmpty())
			continue;
		QLabel* headerLabel = new QLabel(it.key());
		headerLabel->setAlignment(Qt::AlignCenter);
		ui->contacts_layout->addWidget(headerLabel);
		for (auto it2 : it.value())
		{
			ui->contacts_layout->addWidget(it2);
		}
	}
}

void MainWindow::deleteWidgetsFromLayout(QLayout* layout, bool delete_widget)
{
	QLayoutItem* wItem;
	QWidget* widget;
	while ((wItem = layout->takeAt(0)) != NULL) {
		widget = wItem->widget();
		//dont setParent for labels or it will create a new mainWindow
		if (QLabel* label = qobject_cast<QLabel*>(widget))
		{
			layout->removeWidget(widget);
			label->deleteLater();
			continue;
		}
		if (!delete_widget)
			widget->setParent(layout->widget());
		layout->removeWidget(widget);
		if (delete_widget)
			widget->deleteLater();
	}
}

void MainWindow::disableButtons() {
	ui->login_Button->setDisabled(true);
	ui->registration_Button->setDisabled(true);
}
void MainWindow::enableButtons() {
	ui->login_Button->setDisabled(false);
	ui->registration_Button->setDisabled(false);
}
void MainWindow::startLoadingPageAnimation() {
	setScene(UI::Scene::loadingScene);
	ui->loadingPage_loading_label->show();
}

void MainWindow::clearLoadingErrorLabel() { ui->loadingErrorLabel->clear(); }
void MainWindow::clearLoginStatusLabel() { ui->login_statusLabel->clear(); }
void MainWindow::clearRegistrationStatusLabel() { ui->registration_statusLabel->clear(); }
void MainWindow::setLoadingStatus(QString new_status) { ui->loadingStatusLabel->setText(new_status); }
void MainWindow::setLoadingError(QString new_error) { ui->loadingErrorLabel->setText(new_error); }
void MainWindow::setRegistrationStatus(QString new_status) { ui->registration_statusLabel->setText(new_status); }
void MainWindow::setRegistrationError(QString new_error) { ui->registration_statusLabel->setText(QString("<div style='color: red'>%1</div>").arg(new_error)); }
void MainWindow::setLoginStatus(QString new_status) { ui->login_statusLabel->setText(new_status); }
void MainWindow::setLoginError(QString new_error) { ui->login_statusLabel->setText(QString("<div style='color: red'>%1</div>").arg(new_error)); }
void MainWindow::startWelcomePageAnimation() { ui->welcomePage_loading_label->show(); }
void MainWindow::stopLoadingPageAnimation() { ui->loadingPage_loading_label->hide(); }
void MainWindow::stopWelcomePageAnimation() { ui->welcomePage_loading_label->hide(); }
void MainWindow::clearRegistrationPasswordEdit() { ui->registration_PasswordEdit->clear(); }
void MainWindow::clearLoginPasswordEdit() { ui->login_PasswordEdit->clear(); }

void MainWindow::login_button_pressed()
{
	//hide/clear errorLabel
	clearLoginStatusLabel();

	//check if username and password were entered and are long enough
	if (ui->login_UsernameEdit->text().length() < 5) {
		setLoginError(QString("The Username has to be between %1 and %2 Characters long !").arg(QString::number(USERNAME_MIN_LEN), QString::number(USERNAME_MAX_LEN)));
		return;
	}
	else if (ui->login_PasswordEdit->text().length() < 8) {
		setLoginError(QString("The Pasword has to be at least %1 Characters long !").arg(QString::number(PASSWORD_MIN_LEN)));
		return;
	}

	//disable login/registration-button
	disableButtons();

	//show loading animation
	startWelcomePageAnimation();

	//get username, entered password and update statusLabel
	QString username = ui->login_UsernameEdit->text();
	QString unhashed_password = ui->login_PasswordEdit->text();

	//give control to chatbox_client
	client->attemptLogin(username, unhashed_password);
}

void MainWindow::registration_button_pressed()
{
	//hide/clear errorLabels
	clearRegistrationStatusLabel();

	//check if username and password were entered and are long enough
	if (ui->registration_UsernameEdit->text().length() < 5) {
		setRegistrationError(QString("The Username has to be between %1 and %2 Characters long !").arg(QString::number(USERNAME_MIN_LEN), QString::number(USERNAME_MAX_LEN)));
		return;
	}
	else if (ui->registration_PasswordEdit->text().length() < 8) {
		setRegistrationError(QString("The Pasword has to be at least %1 Characters long !").arg(QString::number(PASSWORD_MIN_LEN)));
		return;
	}

	//disable login/registration-button
	disableButtons();

	//show loading animation
	startWelcomePageAnimation();

	//get username, entered password and update statusLabel
	QString username = ui->registration_UsernameEdit->text();
	QString unhashed_password = ui->registration_PasswordEdit->text();

	//give control to chatbox_client
	client->attemptRegistration(username, unhashed_password);
}
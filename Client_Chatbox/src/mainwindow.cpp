#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, client(new Chatbox_Client(this))
{
	ui->setupUi(this);

	setUpSignalSlotConnections();

	setUpUi();

	//do this or with signal (emit establishSocketConnection())?
	//client->establishSocketConnection();
	emit establishSocketConnection_signal();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setUpSignalSlotConnections()
{
	//buttons
	connect(ui->login_Button, SIGNAL(clicked()), this, SLOT(login_button_pressed()));
	connect(ui->registration_Button, SIGNAL(clicked()), this, SLOT(registration_button_pressed()));

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
}

void MainWindow::setUpUi()
{
	ui->chats_grid_layout->setAlignment(Qt::AlignTop);
	ui->contacts_layout->setAlignment(Qt::AlignTop);

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

	setScene(UI::Scene::loadingScene);
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
}

//puts newTopButton to the front of chatButtons-list (adds or re-adds if already in list)
void MainWindow::addTopChatButton(QPushButton* newTopButton)
{
	//remove if exists
	chatButtons.remove(newTopButton);

	//re-add to front
	chatButtons.push_front(newTopButton);
}

void MainWindow::updateChatList()
{
	//delete all buttons from layout
	deleteWidgetsFromLayout(ui->chats_grid_layout->layout());

	//re-add them
	for (const auto& button : chatButtons)
		ui->chats_grid_layout->addWidget(button);
}

void MainWindow::addContact(QString contact)
{
	QPushButton* contactButton = new QPushButton(contact);
	contacts[contact[0]].insert(contact, contactButton);

	updateContactList();
}

void MainWindow::updateContactList()
{
	//clear all widgets in the contact-list
	deleteWidgetsFromLayout(ui->contacts_layout->layout());

	//re-add all contacts + headerLabels, indicating start character
	QMapIterator<QChar, QMap<QString, QPushButton*>> it(contacts);
	while (it.hasNext()) {
		it.next();
		QLabel* headerLabel = new QLabel(it.key());
		headerLabel->setAlignment(Qt::AlignCenter);
		ui->contacts_layout->addWidget(headerLabel);
		for (auto contact_str : it.value().keys())
		{
			ui->contacts_layout->addWidget(new QPushButton(contact_str));
		}
	}
}

void MainWindow::deleteWidgetsFromLayout(QLayout* layout)
{
	QLayoutItem* wItem;
	while ((wItem = layout->takeAt(0)) != NULL) {
		wItem->widget()->setParent(layout->widget());
		delete wItem;
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
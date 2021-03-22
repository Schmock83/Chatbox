#include "fading_information_box.h"
#include "ui_fading_information_box.h"

Fading_Information_Box::Fading_Information_Box(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Fading_Information_Box)
{
	ui->setupUi(this);

	//enable mouse-tracking, so that mousemove-events will fire even when no button is pressed
	setMouseTracking(true);
	ui->label->setMouseTracking(true);

	//intervall for timer
    fadeTimer->setInterval(2000);

    connect(fadeTimer, SIGNAL(timeout()), this, SLOT(fade_out()));

	hide();
}

Fading_Information_Box::~Fading_Information_Box()
{
	delete ui;
}

void Fading_Information_Box::mouseMoveEvent(QMouseEvent* event) {
    fadeTimer->stop();

	//if fade_out-animation is playing -> make it re-appear again
	if (propertyAnimation && propertyAnimation->state() == QPropertyAnimation::Running)
	{
		propertyAnimation = new QPropertyAnimation(eff, "opacity");
		propertyAnimation->setDuration(1);
		propertyAnimation->setStartValue(0);
		propertyAnimation->setEndValue(1);
		propertyAnimation->setEasingCurve(QEasingCurve::InBack);
		propertyAnimation->start();
	}

	show();
    fadeTimer->start();
}

void Fading_Information_Box::mousePressEvent(QMouseEvent* event) {
    fadeTimer->stop();
    hide();
}

//Code from https://stackoverflow.com/questions/19087822/how-to-make-qt-widgets-fade-in-or-fade-out
void Fading_Information_Box::fade_in()
{
	//stop timer from previous fade_in
    fadeTimer->stop();

	this->show();
	this->setGraphicsEffect(eff);
	propertyAnimation = new QPropertyAnimation(eff, "opacity");
	propertyAnimation->setDuration(1000);
	propertyAnimation->setStartValue(0);
	propertyAnimation->setEndValue(1);
	propertyAnimation->setEasingCurve(QEasingCurve::InBack);
	propertyAnimation->start();
	connect(propertyAnimation, SIGNAL(finished()), this, SLOT(faded_in()));
}

void Fading_Information_Box::fade_out()
{
	this->setGraphicsEffect(eff);
	propertyAnimation = new QPropertyAnimation(eff, "opacity");
	propertyAnimation->setDuration(1000);
	propertyAnimation->setStartValue(1);
	propertyAnimation->setEndValue(0);
	propertyAnimation->setEasingCurve(QEasingCurve::OutBack);
	propertyAnimation->start();
	connect(propertyAnimation, SIGNAL(finished()), this, SLOT(faded_out()));
}

void Fading_Information_Box::faded_out()
{
	disconnect(propertyAnimation, SIGNAL(finished()), this, SLOT(faded_out()));
	this->hide();
}
void Fading_Information_Box::faded_in()
{
	disconnect(propertyAnimation, SIGNAL(finished()), this, SLOT(faded_in()));

    fadeTimer->start();
}

void Fading_Information_Box::showInfo(const QString& string)
{
	ui->label->setText(string);
	fade_in();
}

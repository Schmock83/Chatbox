#ifndef FADING_INFORMATION_BOX_H
#define FADING_INFORMATION_BOX_H

//class, that shows a little information-box that fades out after time
//when mouse is moving over it, it will fade back in
//when its clicked, it will disappear

#include <QWidget>

//for opacity effect
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include <QTimer>

namespace Ui {
	class Fading_Information_Box;
}

class Fading_Information_Box : public QWidget
{
	Q_OBJECT

public:
	explicit Fading_Information_Box(QWidget* parent = nullptr);
	~Fading_Information_Box();
	void showInfo(const QString& string);
private:
	Ui::Fading_Information_Box* ui;
    QTimer* fadeTimer = new QTimer(this);
	QGraphicsOpacityEffect* eff = new QGraphicsOpacityEffect(this);
	QPropertyAnimation* propertyAnimation = nullptr;
	void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
public slots:
	void fade_in();
	void fade_out();
	void faded_out();
	void faded_in();
};

#endif // FADING_INFORMATION_BOX_H

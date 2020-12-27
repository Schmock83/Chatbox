#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDataStream>

#include <QDebug>

class Message
{
public:
	Message(QString name = "Name", int age = 0, double money = 0.00);
	friend QDataStream& operator<<(QDataStream& stream, const Message& message) {
		stream << message.name << message.age << message.money;
		return stream;
	}
	friend QDataStream& operator>>(QDataStream& stream, Message& message) {
		stream >> message.name >> message.age >> message.money;
		return stream;
	}
	void print()const {
		qDebug() << name << " - " << age << " - " << money;
	}
private:
	QString name;
	int age;
	double money;
};

#endif // MESSAGE_H

#ifndef CORE_H
#define CORE_H

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1338

//UI-includes
#include <QFile>
#include <QTextStream>
#include <QApplication>

namespace UI {
	bool setUpDarkTheme();
}

//Crypto-includes
#include <sodium.h>

namespace CRYPTO {
	bool hashPassword(char* hashed_password, char* unhashed_password);
	QString hashPassword(const QString& unhashed_password);
	bool verifyPassword(const QString& hashed_password, const QString& password_to_verify);
	bool verifyPassword(const char* hashed_password, const char* password_to_verify);
}
//Helper-Functions
#include <QByteArray>

namespace HELPER {
	char* QStringToCString(const QString& q_str);
	QString CStringToQString(const char* c_str);
}

#endif // CORE_H

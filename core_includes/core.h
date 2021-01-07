#ifndef CORE_H
#define CORE_H

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1338

//database defines

#define USERNAME_MAX_LEN 25
#define USERNAME_MIN_LEN 5
#define PASSWORD_MIN_LEN 8

#define DATABASENAME "qt_server_chatbox.db"
#define USER_TABLE QString("registered_users")
#define CONTACTS_TABLE QString("contacts")

#define USER_TABLE_SCHEME QString("CREATE TABLE IF NOT EXISTS %1 (user_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL DEFAULT(0), user_name VARCHAR(%2) UNIQUE NOT NULL, user_password VARCHAR(150) NOT NULL, registry_date DATETIME DEFAULT CURRENT_TIMESTAMP, last_login DATETIME DEFAULT NULL);").arg(USER_TABLE, QString::number(USERNAME_MAX_LEN))
#define CONTACTS_TABLE_SCHEME QString("CREATE TABLE IF NOT EXISTS %1 (user_id1 INTEGER NOT NULL, user_id2 INTEGER NOT NULL CHECK(user_id1 != user_id2), FOREIGN KEY(user_id1) REFERENCES %2 (user_id) ON UPDATE CASCADE ON DELETE CASCADE, FOREIGN KEY(user_id2) REFERENCES %2 (user_id) ON UPDATE CASCADE ON DELETE CASCADE, PRIMARY KEY(user_id1, user_id2));").arg(CONTACTS_TABLE, USER_TABLE)

//UI-includes
#include <QFile>
#include <QTextStream>
#include <QApplication>

namespace UI {
	//for client
	enum Scene {
		loadingScene = 0,
		welcomeScene,
		mainScene
	};
	//left side (show contacts or chats)
	enum ChatContactPage {
		chatPage = 0,
		contactPage,
		user_search_page
	};
	bool setUpDarkTheme();
}

//Crypto-includes
#include <sodium.h>

namespace CRYPTO {
	class Crypto_Error {
	private:
		QString error_msg;
	public:
		Crypto_Error(QString error_msg)
			:error_msg(error_msg)
		{}
		QString get_error_message()const { return error_msg; }
	};
	bool hashPassword(unsigned char* hashed_password, const unsigned char* unhashed_pass);
	QString hashPassword(const QString& unhashed_pass);
	bool encryptPassword(char* hashed_password, char* unhashed_password);
	QString encryptPassword(const QString& unhashed_password);
	bool verifyPassword(const char* hashed_password, const char* password_to_verify);
	bool verifyPassword(const QString& hashed_password, const QString& password_to_verify);
}

//Helper-Functions
#include <QByteArray>

namespace HELPER {
	char* QStringToCString(const QString& q_str);
	QString CStringToQString(const char* c_str);
	QString UnsignedCharToQString(const unsigned char* str, int length);
	unsigned char* QStringToUnsignedChar(const QString& q_str);
}

#endif // CORE_H

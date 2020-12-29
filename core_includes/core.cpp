#include "core.h"

namespace UI {
	bool setUpDarkTheme() {
		QFile f(":qdarkstyle/style.qss");
		if (!f.exists()) {
			return false;
		}
		else {
			f.open(QFile::ReadOnly | QFile::Text);
			QTextStream ts(&f);
			qApp->setStyleSheet(ts.readAll());
			return true;
		}
	}
}
namespace CRYPTO {
	QString hashPassword(const QString& unhashed_password)//TODO error
	{
		char hashed_password[crypto_pwhash_STRBYTES];
		char* unhashed_password_cstr = HELPER::QStringToCString(unhashed_password);

		if (CRYPTO::hashPassword(hashed_password, unhashed_password_cstr))
			return QString(hashed_password);
	}
	bool hashPassword(char* hashed_password, char* unhashed_password)
	{
		if (crypto_pwhash_str(hashed_password, unhashed_password, strlen(unhashed_password), crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
			//not enough memory -> try again with less memory usage
			return (crypto_pwhash_str(hashed_password, unhashed_password, strlen(unhashed_password), crypto_pwhash_OPSLIMIT_MIN, crypto_pwhash_MEMLIMIT_MIN) == 0);
		}
		return true;
	}
	bool verifyPassword(const char* hashed_password, const char* password_to_verify)
	{
		return (crypto_pwhash_str_verify(hashed_password, password_to_verify, strlen(password_to_verify)) == 0);
	}
	bool verifyPassword(const QString& hashed_password, const QString& password_to_verify)
	{
		char* password_to_verify_cstr = HELPER::QStringToCString(password_to_verify);
		char* hashed_password_cstr = HELPER::QStringToCString(hashed_password);

		return CRYPTO::verifyPassword(hashed_password_cstr, password_to_verify_cstr);
	}
}

namespace HELPER {
	char* QStringToCString(const QString& q_str)
	{
		QByteArray* byteArray = new QByteArray(q_str.toLocal8Bit());
		byteArray->data();
		return byteArray->data();
	}
	QString CStringToQString(const char* c_str)
	{
		return QString(c_str);
	}
}
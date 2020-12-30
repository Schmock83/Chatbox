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
	QString hashPassword(const QString& unhashed_pass) {
		unsigned char hashed_pass_ucstr[crypto_hash_sha512_BYTES];
		char* unhashed_pass_ucstr = HELPER::QStringToCString(unhashed_pass);
		if (crypto_hash_sha512(hashed_pass_ucstr, (unsigned char*)unhashed_pass_ucstr, unhashed_pass.size()) == 0)
			return HELPER::UnsignedCharToQString(hashed_pass_ucstr, crypto_hash_sha512_BYTES);
		else
			return QString();
	}
	bool hashPassword(unsigned char* hashed_password, const unsigned char* unhashed_pass) {
		return (crypto_hash_sha512(hashed_password, unhashed_pass, strlen((char*)unhashed_pass)) == 0);
	}
	QString encryptPassword(const QString& unhashed_password)
	{
		char hashed_password[crypto_pwhash_STRBYTES];
		char* unhashed_password_cstr = HELPER::QStringToCString(unhashed_password);

		if (CRYPTO::encryptPassword(hashed_password, unhashed_password_cstr))
			return QString(hashed_password);
	}
	bool encryptPassword(char* hashed_password, char* unhashed_password)
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
		return byteArray->data();
	}
	QString CStringToQString(const char* c_str)
	{
		return QString(c_str);
	}
	unsigned char* QStringToUnsignedChar(const QString& q_str) {
		char* c_str = HELPER::QStringToCString(q_str);
		return (unsigned char*)c_str;
	}
	QString UnsignedCharToQString(const unsigned char* str, int length) {
		//FROM https://stackoverflow.com/questions/12417210/how-to-convert-unsigned-char-to-qstring
		QString result = "";

		// print string in reverse order
		QString s;
		for (int i = 0; i < length; i++) {
			s = QString("%1").arg(str[i], 0, 16);

			// account for single-digit hex values (always must serialize as two digits)
			if (s.length() == 1)
				result.append("0");

			result.append(s);
		}

		return result;
	}
}
#include "databasehelper.h"

void DatabaseHelper::initiate_sqlite_database()
{
	data_base = QSqlDatabase::addDatabase("QSQLITE");
	data_base.setDatabaseName(DATABASENAME);
	sql_query = QSqlQuery(data_base);
	if (!data_base.open())
		throw data_base.lastError();
	else
		setupTables();
}

void DatabaseHelper::setupTables()
{
	return (setupTable(USER_TABLE_SCHEME));
}

void DatabaseHelper::setupTable(const QString& table_scheme)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(table_scheme);
	if (!sql_query.exec())
		throw data_base.lastError();
}

void DatabaseHelper::register_user(const QString& user_name, const QString& encrypted_password)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(QString("INSERT INTO %1 (user_name, user_password) VALUES (:user_name, :user_password)").arg(USER_TABLE));
	sql_query.bindValue(":user_name", user_name);
	sql_query.bindValue(":user_password", encrypted_password);

	if (!sql_query.exec())
		throw data_base.lastError();
}

bool DatabaseHelper::user_registered(const QString& user_name)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(QString("SELECT EXISTS(SELECT user_id FROM %1 WHERE user_name=:user_name)").arg(USER_TABLE));
	sql_query.bindValue(":user_name", user_name);

	if (!sql_query.exec())
		throw data_base.lastError();
	else
		return (sql_query.first() && sql_query.value(0).toBool());
}

bool DatabaseHelper::verify_user(const QString& user_name, const QString& unhashed_password)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(QString("SELECT user_password FROM %1 WHERE user_name = :usr_name").arg(USER_TABLE));
	sql_query.bindValue(":usr_name", user_name);

	if (!sql_query.exec())
		throw data_base.lastError();

	if (!sql_query.first())
		return false;

	QString stored_password = sql_query.value(0).toString();
	locker.unlock();
	return CRYPTO::verifyPassword(stored_password, unhashed_password);
}
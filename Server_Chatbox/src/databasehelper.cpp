#include "databasehelper.h"

//TODO handle errors

bool DatabaseHelper::initiate_sqlite_database()
{
	data_base = QSqlDatabase::addDatabase("QSQLITE");
	data_base.setDatabaseName(DATABASENAME);
	sql_query = QSqlQuery(data_base);

	return (data_base.open() && setupTables());
}

bool DatabaseHelper::setupTables()
{
	return (setupTable(USER_TABLE_SCHEME));
}

bool DatabaseHelper::setupTable(const QString& table_scheme)
{
	return (sql_query.exec(table_scheme));
}

bool DatabaseHelper::register_user(const QString& user_name, const QString& hashed_password)
{
	if (user_registered(user_name))
		return false;

	sql_query.prepare(QString("INSERT INTO %1 (user_name, user_password) VALUES (:user_name, :user_password)").arg(USER_TABLE));
	sql_query.bindValue(":user_name", user_name);
	sql_query.bindValue(":user_password", hashed_password);

	return (sql_query.exec());
}

bool DatabaseHelper::user_registered(const QString& user_name)
{
	sql_query.prepare(QString("SELECT EXISTS(SELECT user_id FROM %1 WHERE user_name=:user_name)").arg(USER_TABLE));
	sql_query.bindValue(":user_name", user_name);

	return (sql_query.exec() && sql_query.first() && sql_query.value(0).toBool());
}

bool DatabaseHelper::verify_user(const QString& user_name, const QString& unhashed_password)
{
	sql_query.prepare(QString("SELECT user_password FROM %1 WHERE user_name = :usr_name").arg(USER_TABLE));
	sql_query.bindValue(":usr_name", user_name);

	if (!sql_query.exec()) {
		return false;
	}
	return (sql_query.first() && CRYPTO::verifyPassword(sql_query.value(0).toString(), unhashed_password));
}
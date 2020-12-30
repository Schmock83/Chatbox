#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

//include core for defines (database_name + table schema)
#include "../core_includes/core.h"

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

class DatabaseHelper
{
private:
	QSqlDatabase data_base;
	QSqlQuery sql_query;	//for prepared statements

	bool setupTable(const QString& table_scheme);
	bool setupTables();
public:
	DatabaseHelper() {};
	~DatabaseHelper() { close(); }
	bool initiate_sqlite_database();
	void close() { data_base.close(); }
	bool user_registered(const QString& user_name);
	bool register_user(const QString& user_name, const QString& hashed_password);
	bool verify_user(const QString& user_name, const QString& unhashed_password);
};

#endif // DATABASEHELPER_H

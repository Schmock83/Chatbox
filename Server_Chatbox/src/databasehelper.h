#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

//include core for defines (database_name + table schema)
#include "../core_includes/core.h"

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QDateTime>
#include <QList>

class DatabaseHelper
{
private:
	QSqlDatabase data_base;
	QSqlQuery sql_query;	//for prepared statements
	mutable QMutex mutex;

	void setupTable(const QString& table_scheme);
	void setupTables();
public:
	DatabaseHelper() {};
	~DatabaseHelper() { close(); }
	void initiate_sqlite_database();
	void close() { data_base.close(); }
	bool connected() { return data_base.isOpen(); }
	bool construct_user(const QString& user_name, int& user_id, QDateTime& registry_date, QDateTime& last_login);
	bool user_registered(const QString& user_name);
	void register_user(const QString& user_name, const QString& encrypted_password);
	bool verify_user(const QString& user_name, const QString& unhashed_password);
	void update_last_login(const QString& user_name, const QDateTime& loginDateTime);
	int get_user_id(const QString& user_name);
	QList<QString> get_users_like(const QString& user_name);
	QList<QString> get_user_contacts(const int user_id);
	QList<QString> get_user_contacts(const QString& user_name);
	QList<QString> get_user_outgoing_contact_requests(const int user_id);
	QList<QString> get_user_outgoing_contact_requests(const QString& user_name);
	QList<QString> get_user_incoming_contact_requests(const int user_id);
	QList<QString> get_user_incoming_contact_requests(const QString& user_name);
};

#endif // DATABASEHELPER_H

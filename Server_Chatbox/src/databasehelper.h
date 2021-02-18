#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

//include core for defines (database_name + table schema)
#include "../core_includes/core.h"
#include "../core_includes/message.h"

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
	QList<QString> get_user_chats(const QString& user_name); //get all user-names that user_name has chatted with (contacts included)
	QList<QString> get_user_outgoing_contact_requests(const int user_id);
	QList<QString> get_user_outgoing_contact_requests(const QString& user_name);
	QList<QString> get_user_incoming_contact_requests(const int user_id);
	QList<QString> get_user_incoming_contact_requests(const QString& user_name);
	QList<Message> get_stored_user_messages(const QString& user_name);
	QList<Message> get_last_conversation(const QString& user_name1, const QString& user_name2, const QDate& conversation_date = QDate::currentDate());
	void add_user_contact(const int user_id, const int user_id_to_add); //for sending friend request + accepting incoming friend request
	void add_user_contact(const QString& user_name, const QString& user_name_to_add);
	void delete_user_contact(const int user_id, const int user_id_to_del); //for declining friend request + removing user
	void delete_user_contact(const QString& user_name, const QString& user_name_to_del);
	void add_user_message(const Message& message);
	void store_user_message(const Message& message);
	bool user_has_contact(const int user_id, const int contact_user_id);
	bool user_has_contact(const QString& user_name, const QString& contact);
	bool user_has_outgoing_contact_request(const int user_id, const int outgoing_user_id);
	bool user_has_outgoing_contact_request(const QString& user_name, const QString& outgoing_user);
	bool user_has_incoming_contact_request(const int user_id, const int incoming_user_id);
	bool user_has_incoming_contact_request(const QString& user_name, const QString& incoming_user);
};

#endif // DATABASEHELPER_H

#include "user.h"

User::User(QString user_name, DatabaseHelper* database, QTcpSocket* socket)
	: user_name(user_name)
	, database(database)
	, tcp_socket(socket)
{
	//check if user exists-> if not throw exception
	if (!database->construct_user(user_name, user_id, registry_date, last_login))
		throw NoUserInDatabase();
}

void User::update_last_login()
{
	if (database != nullptr && database->connected())
		database->update_last_login(user_name, QDateTime::currentDateTime());
}

QList<QString> User::get_contacts() const
{
	return database->get_user_contacts(user_id);
}
QList<QString> User::get_chats() const
{
	return database->get_user_chats(user_name);
}
QList<QString> User::get_outgoing_contact_requests() const
{
	return database->get_user_outgoing_contact_requests(user_id);
}
QList<QString> User::get_incoming_contact_requests() const
{
	return database->get_user_incoming_contact_requests(user_id);
}

void User::add_user_contact(const int user_id_to_add)
{
	if (valid_database())
		database->add_user_contact(user_id, user_id_to_add);
}
void User::add_user_contact(const QString& user_name)
{
	if (valid_database())
	{
		const int user_id_to_add = database->get_user_id(user_name);
		database->add_user_contact(user_id, user_id_to_add);
	}
}

void User::delete_user_contact(const int user_id_to_del)
{
	if (valid_database())
		database->delete_user_contact(user_id, user_id_to_del);
}
void User::delete_user_contact(const QString& user_name_to_add)
{
	if (valid_database())
	{
		const int user_id_to_del = database->get_user_id(user_name_to_add);
		database->delete_user_contact(user_id, user_id_to_del);
	}
}

bool User::has_contact(const QString& user_name)
{
	if (valid_database())
	{
		const int contact_user_id = database->get_user_id(user_name);
		return database->user_has_contact(user_id, contact_user_id);
	}
}

bool User::has_incoming_contact_request(const QString& user_name)
{
	if (valid_database())
	{
		const int incoming_user_id = database->get_user_id(user_name);
		return database->user_has_incoming_contact_request(user_id, incoming_user_id);
	}
}

bool User::has_outgoing_contact_request(const QString& user_name)
{
	if (valid_database())
	{
		const int outgoing_user_id = database->get_user_id(user_name);
		return database->user_has_outgoing_contact_request(user_id, outgoing_user_id);
	}
}
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
	if (!valid_database())
		return QList<QString>();

	return database->get_user_contacts(user_id);
}
QList<QString> User::get_outgoing_contact_requests() const
{
	if (!valid_database())
		return QList<QString>();

	return database->get_user_outgoing_contact_requests(user_id);
}
QList<QString> User::get_incoming_contact_requests() const
{
	if (!valid_database())
		return QList<QString>();

	return database->get_user_incoming_contact_requests(user_id);
}
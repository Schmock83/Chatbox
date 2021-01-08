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
	setupTable(USER_TABLE_SCHEME);
	setupTable(CONTACTS_TABLE_SCHEME);
}

void DatabaseHelper::setupTable(const QString& table_scheme)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(table_scheme);
	if (!sql_query.exec())
		throw data_base.lastError();
}

int DatabaseHelper::get_user_id(const QString& user_name)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(QString("SELECT user_id FROM %1 WHERE user_name = :user_name").arg(USER_TABLE));
	sql_query.bindValue(":user_name", user_name);

	if (!sql_query.exec())
		throw data_base.lastError();

	//no user found
	if (!sql_query.first())
		return -1;

	return sql_query.value(0).toInt();
}

bool DatabaseHelper::construct_user(const QString& user_name, int& user_id, QDateTime& registry_date, QDateTime& last_login)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(QString("SELECT user_id, registry_date, last_login FROM %1 WHERE user_name = :user_name").arg(USER_TABLE));
	sql_query.bindValue(":user_name", user_name);

	if (!sql_query.exec())
		throw data_base.lastError();

	//no user found
	if (!sql_query.first())
		return false;

	user_id = sql_query.value(0).toInt();
	registry_date = sql_query.value(1).toDateTime();
	last_login = sql_query.value(2).toDateTime();

	return true;
}

QList<QString> DatabaseHelper::get_user_contacts(const int user_id)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(QString("SELECT r.user_name FROM %1 c1 JOIN %2 r ON (r.user_id == c1.user_id2) WHERE c1.user_id1 = :user_id AND c1.user_id2 IN (SELECT user_id1 FROM %1 WHERE user_id2 = :user_id);").arg(CONTACTS_TABLE, USER_TABLE));
	sql_query.bindValue(":user_id", user_id);

	if (!sql_query.exec())
		throw data_base.lastError();

	QList<QString>constacts;

	while (sql_query.next())
	{
		constacts.append(sql_query.value(0).toString());
	}

	return constacts;
}
QList<QString> DatabaseHelper::get_user_contacts(const QString& user_name)
{
	int user_id = get_user_id(user_name);

	return get_user_contacts(user_id);
}
QList<QString> DatabaseHelper::get_user_outgoing_contact_requests(const int user_id)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(
		QString("SELECT r.user_name FROM %1 c1 JOIN %2 r ON(r.user_id == c1.user_id2) WHERE c1.user_id1 = :user_id AND c1.user_id2 NOT IN(SELECT user_id1 FROM %1 WHERE user_id2 = :user_id);").arg(CONTACTS_TABLE, USER_TABLE));
	sql_query.bindValue(":user_id", user_id);

	if (!sql_query.exec())
		throw data_base.lastError();

	QList<QString>outgoing_contact_requests;

	while (sql_query.next())
	{
		outgoing_contact_requests.append(sql_query.value(0).toString());
	}

	return outgoing_contact_requests;
}
QList<QString> DatabaseHelper::get_user_outgoing_contact_requests(const QString& user_name)
{
	int user_id = get_user_id(user_name);

	return get_user_outgoing_contact_requests(user_id);
}
QList<QString> DatabaseHelper::get_user_incoming_contact_requests(const int user_id)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(
		QString("SELECT r.user_name FROM %1 c1 JOIN %2 r ON (r.user_id == c1.user_id1) WHERE c1.user_id2 = :user_id AND c1.user_id1 NOT IN (SELECT user_id2 FROM %1 WHERE user_id1 = :user_id);").arg(CONTACTS_TABLE, USER_TABLE));
	sql_query.bindValue(":user_id", user_id);

	if (!sql_query.exec())
		throw data_base.lastError();

	QList<QString>incomin_friend_requests;

	while (sql_query.next())
	{
		incomin_friend_requests.append(sql_query.value(0).toString());
	}

	return incomin_friend_requests;
}
QList<QString> DatabaseHelper::get_user_incoming_contact_requests(const QString& user_name)
{
	int user_id = get_user_id(user_name);

	return get_user_incoming_contact_requests(user_id);
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
	sql_query.prepare(QString("SELECT user_password FROM %1 WHERE user_name = :user_name").arg(USER_TABLE));
	sql_query.bindValue(":user_name", user_name);

	if (!sql_query.exec())
		throw data_base.lastError();

	if (!sql_query.first())
		return false;

	QString stored_password = sql_query.value(0).toString();
	locker.unlock();
	return CRYPTO::verifyPassword(stored_password, unhashed_password);
}

void DatabaseHelper::update_last_login(const QString& user_name, const QDateTime& loginDateTime)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(QString("UPDATE %1 SET last_login = :date_time WHERE user_name=:user_name").arg(USER_TABLE));
	sql_query.bindValue(":date_time", loginDateTime);
	sql_query.bindValue(":user_name", user_name);

	if (!sql_query.exec())
		throw data_base.lastError();
}
QList<QString> DatabaseHelper::get_users_like(const QString& user_name)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(QString("SELECT user_name FROM %1 WHERE user_name LIKE :user_name").arg(USER_TABLE));
	sql_query.bindValue(":user_name", ("%" + user_name + "%"));

	if (!sql_query.exec())
		throw data_base.lastError();

	QList<QString> found_users;

	while (sql_query.next())
	{
		found_users.append(sql_query.value(0).toString());
	}

	return found_users;
}

void DatabaseHelper::add_user_contact(const int user_id, const int user_id_to_add)
{
	QMutexLocker locker(&mutex);
	sql_query.prepare(QString("INSERT INTO %1 (user_id1, user_id2) VALUES (:user_id1, :user_id2);").arg(CONTACTS_TABLE));
	sql_query.bindValue(":user_id1", user_id);
	sql_query.bindValue(":user_id2", user_id_to_add);

	if (!sql_query.exec())
		throw data_base.lastError();
}

void DatabaseHelper::add_user_contact(const QString& user_name, const QString& user_name_to_add)
{
	const int user_id = get_user_id(user_name);
	const int user_id_to_add = get_user_id(user_name_to_add);

	add_user_contact(user_id, user_id_to_add);
}

void DatabaseHelper::delete_user_contact(const int user_id, const int user_id_to_del)
{
	//case 1 : decline incoming friend request -> remove pair (user_id_to_del, user_id) from contacts
	//case 2 : actually remove a contact -> remove 2 pairs (user_id_to_del, user_id) and (user_id, user_id_to_del) from contacts

	QMutexLocker locker(&mutex);
	sql_query.prepare(QString("DELETE FROM %1 WHERE (user_id1 == :user_id AND user_id2 == :user_id_to_del) OR (user_id1 == :user_id_to_del AND user_id2 == :user_id);").arg(CONTACTS_TABLE));
	sql_query.bindValue(":user_id", user_id);
	sql_query.bindValue(":user_id_to_del", user_id_to_del);

	if (!sql_query.exec())
		throw data_base.lastError();
}

void DatabaseHelper::delete_user_contact(const QString& user_name, const QString& user_name_to_del)
{
	const int user_id = get_user_id(user_name);
	const int user_id_to_del = get_user_id(user_name_to_del);

	delete_user_contact(user_id, user_id_to_del);
}
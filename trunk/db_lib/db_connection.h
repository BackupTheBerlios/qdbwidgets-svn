#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <QSqlDatabase>
#include <QString>

class db_connection
{
  public:
	db_connection(const QString &type, const QString &name);
        ~db_connection();
	bool set_host_name(const QString &host_name);
	bool set_db_name(const QString &db_name);
	bool set_login(const QString &user, const QString &password);
	bool set_connection();
        void close_connection();
	QString last_error() const;	// Return the last error from QSqlDatabe instance (pv_db)
	bool create_test_tables();
	QSqlDatabase get_db() const;	// Return the instance of a QSqlDatabase (connection)
	QString get_cnn_name() const ;

  private:
	// Connexion variables
	QSqlDatabase pv_db;
	QString pv_db_driver;
	QString pv_host_name;
	QString pv_db_name;
	QString pv_cnn_name;	// Name of the connection (passed to QSqlDatabase::addDatabase(type, name)
};

#endif

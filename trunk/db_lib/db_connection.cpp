#include "db_connection.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <iostream>

db_connection::db_connection(const QString &type, const QString &name)
{
  pv_cnn_name = name;
  pv_db = QSqlDatabase::addDatabase(type, name);
  pv_db_driver = type;
  std::cout << "Added database, type: " << type.toStdString().c_str() << ", name: " << name.toStdString().c_str() << std::endl;
}

db_connection::~db_connection()
{
  close_connection();
}

bool db_connection::set_host_name(const QString &host_name)
{
  if(pv_db_driver == "QSQLITE"){
	std::cout << "Host name ignored with QSQLITE driver\n";
	return false;
  }
  pv_db.setHostName(host_name);
  pv_host_name = host_name;
  return true;
}

bool db_connection::set_db_name(const QString &db_name)
{
  pv_db.setDatabaseName(db_name);
  pv_db_name = db_name;
  return true;
}

bool db_connection::set_login(const QString &user, const QString &password)
{
  pv_db.setUserName(user);
  pv_db.setPassword(password);
  return true;
}

bool db_connection::set_connection()
{
  if(!pv_db.open()){
	std::cerr << "Connection failed: " << pv_db.lastError().text().toStdString().c_str() << std::endl;
	return false;
  }
  return true;
}

void db_connection::close_connection()
{
  if(!pv_db.commit()){
    std::cerr << "db_connection::" << __FUNCTION__ << ": commit failed" << std::endl;
    std::cerr << "db_connection::" << __FUNCTION__ << ": Error: " << pv_db.lastError().text().toStdString().c_str() << std::endl;
  }
  std::cerr << "db_connection::" << __FUNCTION__ << ": closing connection name: " << pv_db_name.toStdString().c_str() << std::endl;
  pv_db.close();
}

QString db_connection::last_error() const
{
	return pv_db.lastError().text();
}

QSqlDatabase db_connection::get_db() const
{
	return pv_db;
}

QString db_connection::get_cnn_name() const
{
	return pv_cnn_name;
}

bool db_connection::create_test_tables()
{
  QString SQL;
  SQL = "CREATE TABLE IF NOT EXISTS db_table_TBL ( db_table_name VARCHAR(50) NOT NULL, db_table_field VARCHAR(50) NOT NULL, db_table_field_label VARCHAR(50), PRIMARY KEY(db_table_name, db_table_field));";
  QSqlQuery query;
  if(!query.exec(SQL)){
	std::cout << "SQL error by: " << SQL.toStdString().c_str() << std::endl;
	std::cout << pv_db.lastError().text().toStdString().c_str() << std::endl;
	return false;
  }

  /// qq tests.............
  QString table_name = "product_TBL";
  QString field_name = "product_name";
  QString field_label = "Nom du produit";
  SQL = "SELECT db_table_name, db_table_field FROM db_table_TBL WHERE db_table_name ='" + table_name + "' AND db_table_field ='" + field_name + "'";

  if(!query.exec(SQL)){
	std::cout << "SQL error by: " << SQL.toStdString().c_str() << std::endl;
	std::cout << pv_db.lastError().text().toStdString().c_str() << std::endl;
  }
  // Count num of rows (query.size() is system dependent, so do it manually here)
  int i=0;
  while(query.next()){
	i++;
  }
  if(i < 1){
	// Row don't exists: add it
	SQL = "INSERT INTO db_table_TBL VALUES ('" + table_name + "', '" + field_name + "', '" + field_label + "');";
	if(!query.exec(SQL)){
		std::cout << "SQL error by: " << SQL.toStdString().c_str() << std::endl;
		std::cout << pv_db.lastError().text().toStdString().c_str() << std::endl;
	//	return false;
	}
  }

  // Affichier les resultats
  SQL = "SELECT * FROM db_table_TBL;";
  if(!query.exec(SQL)){
	std::cout << "SQL error by: " << SQL.toStdString().c_str() << std::endl;
	std::cout << pv_db.lastError().text().toStdString().c_str() << std::endl;
  }
  while(query.next()){
	//QString id = query.value(0).toString();
	//QString txt = query.value(1).toString();
	//std::cout  << id.toStdString().c_str() << ": " << txt.toStdString().c_str() << "\n";
	QSqlRecord rec = query.record();
	int i=0;
	for(i=0; i<rec.count(); i++){
		QString name = rec.fieldName(i);
		QString txt = rec.value(i).toString();
		std::cout << name.toStdString().c_str() << ": " << txt.toStdString().c_str() << "\t";
	}
	std::cout << std::endl;
  }
  return true;
}

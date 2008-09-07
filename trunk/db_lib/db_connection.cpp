/***************************************************************************
 *   Copyright (C) 2008 by Philippe   *
 *   nel230@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "db_connection.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <iostream>

db_connection::db_connection(const QString &type, const QString &name)
{
  set_db_driver(type, name);
}

db_connection::db_connection()
{
}

db_connection::~db_connection()
{
  close_connection();
}

bool db_connection::set_db_driver(const QString &type, const QString &name)
{
  pv_cnn_name = name;
  if(pv_db.isOpen()){
    close_connection();
  }
  pv_db = QSqlDatabase::addDatabase(type, name);
  pv_db_driver = type;
  if(!pv_db.isValid()){
    std::cerr << "db_connection::" << __FUNCTION__ << ": Cannot add database, type: " << type.toStdString() << 
      " , name: " << name.toStdString() << std::endl;
    return false;
  }
  return true;
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

bool db_connection::set_port(const QString &port)
{
  int i_port = 0;
  bool conv_ok = false;
  if(pv_db_driver == "QSQLITE"){
	std::cout << "port ignored with QSQLITE driver\n";
	return false;
  }
  // Convert string to int
  i_port = port.toInt(&conv_ok, 10);
  if(!conv_ok){
    std::cerr << "db_connection::" << __FUNCTION__ << ": convertion of port to int failed: given string: " << port.toStdString() << std::endl;
    return false;
  }
  pv_db.setPort(i_port);
  return true;
}

bool db_connection::set_db_name(const QString &db_name)
{
  pv_db.setDatabaseName(db_name);
  pv_db_name = db_name;
  return true;
}

bool db_connection::set_login(const QString user, const QString password)
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
  if(!pv_db.isOpen()){
	std::cerr << "Connection failed: " << pv_db.lastError().text().toStdString().c_str() << std::endl;
	return false;
  }
  if(pv_db.isOpenError()){
	std::cerr << "Connection failed: " << pv_db.lastError().text().toStdString().c_str() << std::endl;
	return false;
  }
  return true;
}

bool db_connection::is_open() const
{
  return pv_db.isOpen();
}

void db_connection::close_connection()
{
  if(!pv_db.isOpen()){
    return;
  }
  if(!pv_db.commit()){
    std::cerr << "db_connection::" << __FUNCTION__ << ": commit failed" << std::endl;
    std::cerr << "db_connection::" << __FUNCTION__ << ": Error: " << pv_db.lastError().text().toStdString().c_str() << std::endl;
  }
  std::cerr << "db_connection::" << __FUNCTION__ << ": closing connection name: " << pv_cnn_name.toStdString() << std::endl;
  QSqlDatabase::removeDatabase(pv_cnn_name);
  pv_databases.clear();
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

QString db_connection::get_driver_type() const
{
  return pv_db_driver;
}

QString db_connection::get_host_name() const
{
  return pv_db.hostName();
}

QString db_connection::get_port() const
{
  QString port;
  port.setNum(pv_db.port());
  return port;
}

QString db_connection::get_db_name() const
{
  //return pv_db.databaseName();
  return pv_db_name;
}

QString db_connection::get_login_user() const
{
  return pv_db.userName();
}

QString db_connection::get_login_passwd() const
{
  return pv_db.password();
}

QString db_connection::get_cnn_name() const
{
  return pv_cnn_name;
}

QStringList db_connection::get_tables() const
{
  return pv_db.tables();
}

QStringList db_connection::get_databases() const
{
  list_databases();
  return pv_databases;
}

bool db_connection::list_databases()
{
  QString SQL;
  SQL = "SHOW DATABASES;";
  QSqlQuery query(pv_db);
  if(!query.exec(SQL)){
    std::cerr << "db_connection::" << __FUNCTION__ << ": SHOW DATABASES; query failed" << std::endl;
    std::cerr << pv_db.lastError().text().toStdString().c_str() << std::endl;
    return false;
  }
  while(query.next()){
    QString data = query.value(0).toString();
    pv_databases << data;
  }
  return true;
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

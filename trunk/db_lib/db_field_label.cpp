/***************************************************************************
 *   Copyright (C) 2007 by Philippe   *
 *   nel230@gmail.ch   *
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

#include "db_field_label.h"
#include <iostream>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
//#include <QSqlDatabase>

db_field_label::db_field_label()
{
  //pv_cnn = 0;
  pv_query = 0;
  pv_rc_query = 0;
  //std::cerr << "db_field_label::" << __FUNCTION__ << ": default constructor. Don't forget to use init call" << std::endl;
}

db_field_label::db_field_label(const db_connection *cnn, const QString &table_name)
{
  init(cnn, table_name);
}

bool db_field_label::init(const db_connection *cnn, const QString &table_name)
{
  pv_table_name = table_name;
  pv_cnn = cnn;
  pv_cnn_qname = cnn->get_cnn_name();
  pv_cnn_cname = cnn->get_cnn_name().toStdString().c_str();

  pv_query = new QSqlQuery(pv_cnn->get_db());
  pv_rc_query = new QSqlQuery(pv_cnn->get_db());

  // System table
  if(!create_sys_table()){
	std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": error by system table creation" << std::endl;
	return false;
  }
  return true;
}

void db_field_label::set_table_name(const QString & table_name)
{
  pv_table_name = table_name;
}

bool db_field_label::set_label(const QString &field_name, const QString &field_label)
{
  if(entry_exists(field_name)){
    // Update entry
    pv_SQL = "UPDATE "  + pv_sys_table_name + \
    " SET db_table_field_label ='" + field_label + \
    "' WHERE db_table_name ='" + pv_table_name + "' AND db_table_field ='" + field_name + "';";
    if(!pv_query->exec(pv_SQL)){
          std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": SQL error by:\n\t" << pv_SQL.toStdString().c_str() << std::endl;
          std::cerr << pv_query->lastError().text().toStdString().c_str() << std::endl;
          return false;
    }
  }else{
    // Add the entry
    pv_SQL = "INSERT INTO " + pv_sys_table_name + \
            " (db_table_name, db_table_field, db_table_field_label) VALUES ('" \
            + pv_table_name + "','" + field_name + "','" + field_label + "');";
    if(!pv_query->exec(pv_SQL)){
          std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": SQL error by:\n\t" << pv_SQL.toStdString().c_str() << std::endl;
          std::cerr << pv_query->lastError().text().toStdString().c_str() << std::endl;
          return false;
    }
  }
  return true;
}

QString db_field_label::get_label(const QString &field_name)
{
  pv_SQL = "SELECT db_table_field_label "  \
  " FROM " + pv_sys_table_name + \
  " WHERE db_table_name ='" + pv_table_name + "' AND db_table_field ='" + field_name + "';";
  if(!pv_query->exec(pv_SQL)){
    std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": SQL error by:\n\t" << pv_SQL.toStdString().c_str() << std::endl;
    std::cerr << pv_query->lastError().text().toStdString().c_str() << std::endl;
    return field_name;
  }
  if(!pv_query->next()){
    //std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": No row found" << std::endl;
    return field_name;
  }
  QSqlRecord rec = pv_query->record();
  return rec.value(0).toString();
}

void db_field_label::tests()
{
  pv_SQL = "SELECT * FROM " + pv_sys_table_name + ";";
  if(!pv_query->exec(pv_SQL)){
	std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": SQL error by:\n\t" << pv_SQL.toStdString().c_str() << std::endl;
	std::cerr << pv_query->lastError().text().toStdString().c_str() << std::endl;
  }
  // Display data
  int i=0;
  while(pv_query->next()){
	QSqlRecord rec = pv_query->record();
	for(i=0; i<rec.count(); i++){
		QString name = rec.value(i).toString();
		std::cout << name.toStdString().c_str() << "\t";
	}
	std::cout << "\n";
  }
  row_count(pv_SQL);

  std::cerr << "Label pour client_description: " << qPrintable(get_label("client_description")) << std::endl;
}

bool db_field_label::create_sys_table(const QString & sys_table_name)
{
  /// The "system" table name
  if(sys_table_name.isEmpty()){
    pv_sys_table_name = "db_sys_field_name_TBL";
  }else{
    pv_sys_table_name = sys_table_name;
  }
  // Tests if table allready exists
  if(!table_exists(pv_sys_table_name)){
    // Create the table
    std::cout << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": Creating system table\n";
    pv_SQL = "CREATE TABLE "/*IF NOT EXISTS*/ + pv_sys_table_name + \
            " ( db_table_name VARCHAR(50) NOT NULL, db_table_field VARCHAR(50) NOT NULL," \
            " db_table_field_label VARCHAR(50), PRIMARY KEY(db_table_name, db_table_field));";
    if(!pv_query->exec(pv_SQL)){
          std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": SQL error by:\n\t" << pv_SQL.toStdString().c_str() << std::endl;
          std::cerr << pv_query->lastError().text().toStdString().c_str() << std::endl;
          return false;
    }
  }
  return true;
}

bool db_field_label::table_exists(const QString &table_name)
{
  // See if the table exists
  pv_SQL = "SELECT * FROM " + pv_sys_table_name + ";";
  //std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": execute: " << pv_SQL.toStdString().c_str() << std::endl;
  if(!pv_query->exec(pv_SQL)){
	std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": SQL error by:\n\t" << pv_SQL.toStdString().c_str() << std::endl;
	std::cerr << pv_query->lastError().text().toStdString().c_str() << std::endl;
	return false;
  }
  if(row_count(pv_SQL) >= 1){
        //std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": found" << std::endl;
	return true;
  }
  std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": not found" << std::endl;
  return false;
}

bool db_field_label::entry_exists(const QString &field_name)
{
  // See if the entry exists
  pv_SQL = "SELECT * FROM " + pv_sys_table_name + " WHERE db_table_name='" + pv_table_name \
	   + "' AND db_table_field='" + field_name + "';";
  if(!pv_query->exec(pv_SQL)){
	std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": SQL error by:\n\t" << pv_SQL.toStdString().c_str() << std::endl;
	std::cerr << pv_query->lastError().text().toStdString().c_str() << std::endl;
	return false;
  }
  if(row_count(pv_SQL) >= 1){
	return true;
  }
  return false;
}

int db_field_label::row_count(const QString SQL)
{
  if(!pv_rc_query->exec(SQL)){
	std::cerr << pv_cnn_cname << "->db_field_label::" << __FUNCTION__ << ": SQL error by:\n\t" << SQL.toStdString().c_str() << std::endl;
	std::cerr << pv_rc_query->lastError().text().toStdString().c_str() << std::endl;
	return -1;
  }
  // Count the rows
  int i=0;
  while(pv_rc_query->next()){
	i++;
        //std::cout << "ROWS\n";
  }
  return i;
}

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

#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <QSqlDatabase>
#include <QString>
#include <QStringList>

class db_connection
{
 public:
  db_connection(const QString &type, const QString &name);
  db_connection();
  ~db_connection();
  bool set_db_driver(const QString &type, const QString &name);
  bool set_host_name(const QString &host_name);
  bool set_port(const QString &port);
  bool set_db_name(const QString &db_name);
  bool set_login(const QString user, const QString password);
  bool set_connection();
  // Return true if db is open
  bool is_open() const;
  void close_connection();
  QString last_error() const;	// Return the last error from QSqlDatabe instance (pv_db)
  bool create_test_tables();
  QSqlDatabase get_db() const;	// Return the instance of a QSqlDatabase (connection)
  QString get_driver_type() const;
  QString get_cnn_name() const ;
  QString get_host_name() const;
  QString get_port() const;
  QString get_db_name() const;
  QString get_login_user() const;
  QString get_login_passwd() const;
  QStringList get_databases() const;
  QStringList get_tables() const;

 private:
  // Connexion variables
  QSqlDatabase pv_db;
  QString pv_db_driver;
  QString pv_host_name;
  QString pv_db_name;
  QString pv_cnn_name;	// Name of the connection (passed to QSqlDatabase::addDatabase(type, name)
  bool list_databases();
  QStringList pv_databases;
};

#endif

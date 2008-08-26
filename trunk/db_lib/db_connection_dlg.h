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

#ifndef DB_CONNECTION_DLG_H
#define DB_CONNECTION_DLG_H

#include <QDialog>
#include <QWidget>
#include <QString>

#include "db_connection.h"

class QComboBox;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class db_connection_dlg : public QDialog
{
 Q_OBJECT

 public:
  db_connection_dlg(const QString & name, QWidget *parent = 0);
  ~db_connection_dlg();
  bool set_dbc(const db_connection *dbc);
  db_connection *get_dbc() const;

 private slots:
  void db_choosed(const QString &text);
  void select_file();
  void connect_db();
  void disconnect_db();
  void list_tables();
  void use_db();
  void ok();
  void cancel();

 private:
  void set_sqlite_layout();
  void set_mysql_layout();

  QHBoxLayout *pv_hmain_layout;
  QVBoxLayout *pv_vleft_layout;
  QVBoxLayout *pv_vright_layout;
  QLabel *lb_db_driver;
  QComboBox *cb_db_driver;
  QLabel *lb_db;
  QComboBox *cb_db;
  // file db (QSQLITE)
  QHBoxLayout *pv_dbfile_layout;
  QLabel *lb_db_file;
  QLineEdit *le_db_file;
  QPushButton *pb_db_file;
  // Host
  QLabel *lb_host;
  QLineEdit *le_host;
  QLabel *lb_port;
  QLineEdit *le_port;
  QLabel *lb_user;
  QLineEdit *le_user;
  QLabel *lb_pass;
  QLineEdit *le_pass;
  // Connect
  QPushButton *pb_connect;
  db_connection *pv_dbc;
  QLabel *lb_connect_status;
  QLabel *lb_msg;
  // Disonnect
  QPushButton *pb_disconnect;
  // List tables
  QPushButton *pb_list_tables;
  QComboBox *cb_tables;
  // Db to use
  QPushButton *pb_use_db;
  // Ok / cancel
  QPushButton *pb_ok;
  QPushButton *pb_cancel;
};

#endif

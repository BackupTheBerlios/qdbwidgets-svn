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

#include "db_connection_dlg.h"
#include "db_connection.h"

#include <iostream>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

db_connection_dlg::db_connection_dlg(const QString & name, QWidget *parent)
  : QDialog(parent)
{
  // Base layout
  pv_hmain_layout = new QHBoxLayout;
  pv_vleft_layout = new QVBoxLayout;
  pv_vright_layout = new QVBoxLayout;
  pv_hmain_layout->addLayout(pv_vleft_layout);
  pv_hmain_layout->addLayout(pv_vright_layout);
  // Driver cbox
  lb_db_driver = new QLabel(tr("Database driver"));
  pv_vleft_layout->addWidget(lb_db_driver);
  cb_db_driver = new QComboBox;
  cb_db_driver->addItem(tr("Please choose"));
  cb_db_driver->addItem("QSQLITE");
  cb_db_driver->addItem("QMYSQL");
  pv_vleft_layout->addWidget(cb_db_driver);
  connect(cb_db_driver, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(db_choosed(const QString&)) );
  // List tables
  pb_list_tables = new QPushButton(tr("List tables"));
  pv_vright_layout->addWidget(pb_list_tables);
  cb_tables = new QComboBox;
  pv_vright_layout->addWidget(cb_tables);
  connect(pb_list_tables, SIGNAL(clicked()), this, SLOT(list_tables()) );
  // Database cbox
  lb_db = new QLabel(tr("Aviable databases        "));
  pv_vright_layout->addWidget(lb_db);
  cb_db = new QComboBox;
  //cb_db->setEditable(true);
  pv_vright_layout->addWidget(cb_db);
  // db file chooser cbox
  pv_dbfile_layout = new QHBoxLayout;
  lb_db_file = new QLabel(tr("Database file"));
  //pv_dbfile_layout->addWidget(lb_db_file);
  pv_vleft_layout->addWidget(lb_db_file);
  le_db_file = new QLineEdit;
  pv_dbfile_layout->addWidget(le_db_file);
  pb_db_file = new QPushButton("...");
  pv_dbfile_layout->addWidget(pb_db_file);
  pv_vleft_layout->addLayout(pv_dbfile_layout);
  connect(pb_db_file, SIGNAL(clicked()), this, SLOT(select_file()) );
  // host, port, user, pass
  QHBoxLayout *host_port_lb_layout = new QHBoxLayout;
  QHBoxLayout *host_port_le_layout = new QHBoxLayout;
  lb_host = new QLabel(tr("Host"));
  host_port_lb_layout->addWidget(lb_host);
  lb_port = new QLabel(tr("Port"));
  host_port_lb_layout->addWidget(lb_port);
  pv_vleft_layout->addLayout(host_port_lb_layout);
  le_host = new QLineEdit;
  host_port_le_layout->addWidget(le_host);
  le_port = new QLineEdit;
  host_port_le_layout->addWidget(le_port);
  pv_vleft_layout->addLayout(host_port_le_layout);
  lb_user = new QLabel(tr("User"));
  pv_vleft_layout->addWidget(lb_user);
  le_user = new QLineEdit;
  pv_vleft_layout->addWidget(le_user);
  lb_pass = new QLabel(tr("Password"));
  pv_vleft_layout->addWidget(lb_pass);
  le_pass = new QLineEdit;
  le_pass->setEchoMode(QLineEdit::Password);
  pv_vleft_layout->addWidget(le_pass);
  // Db to use
  pb_use_db = new QPushButton(tr("Use this db"));
  pv_vright_layout->addWidget(pb_use_db);
  connect(pb_use_db, SIGNAL(clicked()), this, SLOT(use_db()) );
  // connect
  pv_dbc = 0;
  lb_connect_status = new QLabel(tr("No database connection set"));
  pv_vright_layout->addWidget(lb_connect_status);
  pb_connect = new QPushButton(tr("Connect"));
  pv_vleft_layout->addWidget(pb_connect);
  connect(pb_connect, SIGNAL(clicked()), this, SLOT(connect_db()) );
  // disconnect
  pb_disconnect = new QPushButton(tr("Disconnect"));
  pv_vleft_layout->addWidget(pb_disconnect);
  connect(pb_disconnect, SIGNAL(clicked()), this, SLOT(disconnect_db()) );
  // Messages
  lb_msg = new QLabel(tr("Ready"));
  pv_vleft_layout->addWidget(lb_msg);

  db_choosed(tr("Please choose"));
  pv_vright_layout->addStretch();
  // Ok /  Cancel
  pb_ok = new QPushButton(tr("Ok"));
  pv_vright_layout->addWidget(pb_ok);
  connect(pb_ok, SIGNAL(clicked()), this, SLOT(ok()));
  pb_cancel = new QPushButton(tr("Cancel"));
  pv_vright_layout->addWidget(pb_cancel);
  connect(pb_cancel, SIGNAL(clicked()), this, SLOT(cancel()));
  setLayout(pv_hmain_layout);
}

db_connection_dlg::~db_connection_dlg()
{
}

db_connection *db_connection_dlg::get_dbc() const
{
  if(pv_dbc != 0){
    if(pv_dbc->is_open()){
      return pv_dbc;
    }
  }
  return 0;
}

void db_connection_dlg::db_choosed(const QString &text)
{
  // Initial
  le_db_file->setReadOnly(true);
  le_host->setReadOnly(true);
  le_port->setReadOnly(true);
  le_user->setReadOnly(true);
  le_pass->setReadOnly(true);
  pb_db_file->setEnabled(false);
  pb_use_db->setEnabled(false);

  QString s_sqlite("QSQLITE");
  QString s_smysql("QMYSQL");

  if(text == s_sqlite){
    std::cout << "Database: " << text.toStdString() << " : " << s_sqlite.toStdString() << std::endl;
    set_sqlite_layout();
  }else if(text == s_smysql){
    set_mysql_layout();
  }
}

void db_connection_dlg::set_sqlite_layout()
{
  le_db_file->setReadOnly(false);
  pb_db_file->setEnabled(true);
}

void db_connection_dlg::select_file()
{
  QString file;
  file = QFileDialog::getOpenFileName(this, tr("Open databse"), ".", tr("Database files (*.db)"));
  if(!file.isEmpty()){
    le_db_file->setText(file);
  }
}

void db_connection_dlg::set_mysql_layout()
{
  le_host->setReadOnly(false);
  le_host->setText("localhost");
  le_user->setReadOnly(false);
  le_pass->setReadOnly(false);
  le_port->setReadOnly(false);
  le_port->setText("3306");
}

void db_connection_dlg::connect_db()
{
  QString s_driver;
  lb_msg->setText(tr("Ready"));
  if(pv_dbc != 0){
    disconnect_db();
  }else{
    pv_dbc = new db_connection();
  }
  // get driver text from driver comboBox
  s_driver = cb_db_driver->currentText();
  // Sqlite connection
  if(s_driver == "QSQLITE"){
    cb_db->clear();
    if(le_db_file->text().isEmpty()){
      lb_connect_status->setText(tr("Please set the database file"));
      return;
    }
    if(!pv_dbc->set_db_driver(s_driver, s_driver)){
      lb_connect_status->setText(tr("Unable to set database driver"));
    }
    if(pv_dbc->set_db_name(le_db_file->text())){
      lb_connect_status->setText(tr("Connected"));
    }else{
      lb_connect_status->setText(tr("Connection failed"));
    }
  // MySql connection
  }else if(s_driver == "QMYSQL"){
    if(le_host->text().isEmpty()){
      lb_connect_status->setText(tr("Please set hostname"));
      return;
    }
    if(le_port->text().isEmpty()){
      lb_connect_status->setText(tr("Please set port"));
      return;
    }
    if(le_user->text().isEmpty()){
      lb_connect_status->setText(tr("Please set username"));
      return;
    }
    if(le_pass->text().isEmpty()){
      lb_connect_status->setText(tr("Please set password"));
      return;
    }
    if(!pv_dbc->set_db_driver(s_driver, s_driver)){
      lb_connect_status->setText(tr("Unable to set database driver"));
    }
    // Set hostname
    if(pv_dbc->set_host_name(le_host->text())){
      lb_connect_status->setText(tr("Host found..."));
    }else{
      lb_connect_status->setText(tr("Failed to set hostname"));
      return;
    }
    // Set port
    if(pv_dbc->set_port(le_port->text())){
      lb_connect_status->setText(tr("Port set..."));
    }else{
      lb_connect_status->setText(tr("Failed to set port"));
      return;
    }
    if(pv_dbc->set_login(le_user->text(), le_pass->text())){
      lb_connect_status->setText(tr("Login Ok"));
    }else{
      lb_connect_status->setText(tr("Failed to login"));
      return;
    }
    // If a database name wa set in cb_db, take it
    if(!cb_db->currentText().isEmpty()){
      if(pv_dbc->set_db_name(cb_db->currentText())){
        lb_connect_status->setText(tr("Database set"));
      }else{
        lb_connect_status->setText(tr("Unable to set database"));
      }
    }
    cb_db->clear();
  // No known driver was selected
  }else{
    lb_connect_status->setText(tr("Please choose database driver"));
    return;
  }
  // Here: all needed data must be set, try to connect
  if(pv_dbc->set_connection()){
    lb_connect_status->setText(tr("Connected"));
    lb_msg->setText(tr("Connected, database name: ") + pv_dbc->get_cnn_name());
  }else{
    lb_connect_status->setText(tr("Connection failed"));
    lb_msg->setText(pv_dbc->last_error());
    return;
  }
  // Ok, we should be connected here
 cb_db->addItems(pv_dbc->get_databases());
 if(cb_db->count() > 0){
    pb_use_db->setEnabled(true);
 }
}

void db_connection_dlg::disconnect_db()
{
  if(pv_dbc != 0){
    pv_dbc->close_connection();
    lb_connect_status->setText(tr("No database connection set"));
    lb_msg->setText(tr("Ready"));
    pb_use_db->setEnabled(false);
    cb_tables->clear();
  }
}

void db_connection_dlg::list_tables()
{
  int i = 0;
  QStringList tables;
  cb_tables->clear();
  if(pv_dbc != 0){
    tables = pv_dbc->get_tables();
    if(tables.empty()){
      lb_msg->setText(pv_dbc->last_error());
    }
    cb_tables->addItems(tables);
  }
}

void db_connection_dlg::use_db()
{
  // Simply recall connect_db()
  connect_db();
}

void db_connection_dlg::ok()
{
  accept();
}

void db_connection_dlg::cancel()
{
  disconnect_db();
  reject();
}

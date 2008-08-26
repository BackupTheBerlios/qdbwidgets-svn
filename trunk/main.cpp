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

#include <QApplication>
#include <QDialog>
#include <iostream>

#include "db_lib/db_connection.h"
#include "db_lib/db_connection_dlg.h"
#include "db_lib/db_query_widget.h"
#include "db_lib/db_table_widget.h"
#include "db_lib/db_tab_widget.h"
#include "db_lib/db_search_widget.h"


#include "main_dialog.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  int retval = 0;
  db_connection *dbc = 0;
  main_dialog md("test");

  /// Tests
/*
  dbc = new db_connection("QSQLITE", "test");
  dbc->set_db_name("qwidget_test.db");
*/
  dbc = new db_connection("QMYSQL", "test");
  //dbc->set_db_name("scandyna");
  dbc->set_login("scandyna","");

  db_connection_dlg dbc_dlg("Connexion");
  dbc_dlg.set_dbc(dbc);
  retval = dbc_dlg.exec();
  //std::cout << "Return: " << retval << std::endl;
  if(retval == QDialog::Accepted){
    dbc = dbc_dlg.get_dbc();
  }else{
    return 1;
  }

  if(dbc != 0){
    md.init(dbc, "client_TBL");
    md.show();
  }else{
    return 1;
  }

  return app.exec();
}

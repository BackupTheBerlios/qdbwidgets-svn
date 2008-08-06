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
#include <iostream>

#include "db_lib/db_connection.h"
#include "db_lib/db_query_widget.h"
#include "db_lib/db_table_widget.h"
#include "db_lib/db_tab_widget.h"
#include "db_lib/db_search_widget.h"


#include "main_dialog.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  // Mysql connection
/*  db_connection dbc("QMYSQL", "mysql");
  dbc.set_host_name("127.0.0.1");
  dbc.set_db_name("database");
  dbc.set_login("user","password");
  if(!dbc.set_connection()){
    std::cerr << "Main: connection failed\n";
    return -1;
  }
*/
  // Sqlite3 connection
  db_connection dbc("QSQLITE", "sqlite");
  dbc.set_db_name("qwidget_test.db");
  dbc.set_connection();
  if(!dbc.set_connection()){
    std::cerr << "Main: connection failed\n";
    return -1;
  }

  main_dialog md("test");
  md.init(&dbc, "client_TBL");
  md.show();

  return app.exec();
}

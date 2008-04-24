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

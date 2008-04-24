#ifndef DB_FIELD_LABEL_H
#define DB_FIELD_LABEL_H

/// This class is usefull to display a label
/// The correspondance between table <-> filed
/// are stored in a "system" table

#include <QString>
#include <QSqlQuery>
#include "db_connection.h"
//class QSqlDatabase;

class db_field_label
{
 public:
  /// Call init() function if you use this default constructor
  db_field_label();
  /// Give a valid connection and the table name in witch you need field labels
  db_field_label(const db_connection *cnn, const QString &table_name);
  /// Create system table
  bool create_sys_table(const QString & sys_table_name=0);
  bool init(const db_connection *cnn, const QString &table_name = 0);
  /// Change table name
  void set_table_name(const QString & table_name);
  /// Add a label to a field
  bool set_label(const QString &field_name, const QString &field_label);
  /// Get the label name of a given field_name
  QString get_label(const QString &field_name);

  void tests();

 private:
  bool table_exists(const QString &table_name);
  bool entry_exists(const QString &field_name);
  int row_count(const QString SQL);
  //QSqlDatabase pv_cnn;		// Pointer to the database connection instance
  const db_connection *pv_cnn;
  QString pv_sys_table_name;
  QString pv_table_name;	// The name of current used table
  QSqlQuery *pv_query;		// Private instance of QSqlQuery
  QString pv_SQL;		// Contains the SQL query string
  QSqlQuery *pv_rc_query;	// Same, but for row_cout() function
  QString pv_cnn_qname;		// Database connection name
  const char *pv_cnn_cname;
};

#endif

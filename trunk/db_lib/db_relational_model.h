#ifndef DB_RELATIONAL_MODEL
#define DB_RELATIONAL_MODEL

#include <QWidget>
#include <QSqlRelationalTableModel>

#include "db_field_label.h"

class QString;
class QStringList;

class db_relational_model : public QSqlRelationalTableModel
{
 Q_OBJECT

 public:
  //db_relational_model(QObject * parent = 0, QSqlDatabase db = QSqlDatabase());
  db_relational_model(QObject * parent = 0, QSqlDatabase db = QSqlDatabase());
  /// Call this init function before any use - don't use "setTable() ! "
  bool init(const db_connection *cnn, const QString &table_name);
  /// Set user friendly headers (found with db_field_label)
  void set_user_headers();
  void clear_filter();
  bool add_filter(const QString &field_name, const QString &val);

 private:
  db_field_label pv_label;          // Field labels
  QString pv_filter;
};

#endif // define DB_RELATIONAL_MODEL

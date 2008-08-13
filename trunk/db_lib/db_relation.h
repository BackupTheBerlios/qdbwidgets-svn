#ifndef DB_RELATION_H
#define DB_RELATION_H

#include <QString>
#include <QStringList>

/// This class simply holds the relation field names
/// of parent and child table
class db_relation
{
 public:
  db_relation(const QString & tableName);
  /// add a filed to relation concerned by parent table
  void add_parent_relation_field(const QString &field_name);
  QStringList get_parent_relation_fields();
  /// add a filed to relation concerned by child table
  void add_child_relation_field(const QString &field_name);
  QStringList get_child_relation_fields();

 private:
  QStringList pv_parent_table_relations;
  QStringList pv_child_table_relations;
  QString pv_table_name;
};

#endif

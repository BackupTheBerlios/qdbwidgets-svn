#include "db_relation.h"

db_relation::db_relation(const QString & table_name)
{
  pv_table_name = table_name;
}

void db_relation::add_parent_relation_field(const QString &field_name)
{
  pv_parent_table_relations << field_name;
}

QStringList db_relation::get_parent_relation_fields()
{
  return pv_parent_table_relations;
}

void db_relation::add_child_relation_field(const QString &field_name)
{
  pv_child_table_relations << field_name;
}

QStringList db_relation::get_child_relation_fields()
{
  return pv_child_table_relations;
}

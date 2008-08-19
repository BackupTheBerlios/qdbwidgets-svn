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

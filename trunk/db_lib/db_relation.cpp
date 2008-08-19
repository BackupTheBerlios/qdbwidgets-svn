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

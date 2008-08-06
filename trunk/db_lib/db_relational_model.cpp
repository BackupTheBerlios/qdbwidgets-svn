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

#include "db_relational_model.h"
#include <iostream>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QSqlRecord>
#include <QSqlField>

db_relational_model::db_relational_model(QObject * parent, QSqlDatabase db)
  : QSqlRelationalTableModel(parent, db)
{

}

bool db_relational_model::init(const db_connection *cnn, const QString &table_name)
{
  // Init the labels
  if(!pv_label.init(cnn, table_name)){
    std::cerr << "db_relational_model::" << __FUNCTION__ << ": pv_label.init() failed" << std::endl;
    return false;
  }
  setTable(table_name);
  return true;
}

void db_relational_model::set_user_headers()
{
  int i=0;
  // Set the header data - take from field_label class instance
  QStringList lst;
  for(i=0; i<columnCount() ;i++){
    lst.insert(i, headerData(i, Qt::Horizontal).toString());
    setHeaderData(i , Qt::Horizontal, pv_label.get_label(lst.at(i)));
  }
}

void db_relational_model::clear_filter()
{
  pv_filter.clear();
  setFilter(pv_filter);
}

bool db_relational_model::add_filter(const QString &field_name, const QString &val)
{
  if(field_name.isEmpty()){
    std::cerr << "db_relational_model::" << __FUNCTION__ << ": no fild name given" << std::endl;
    return false;
  }
  if(val.isEmpty()){
    std::cerr << "db_relational_model::" << __FUNCTION__ << ": no criteria value given" << std::endl;
    // Set an invalide value
    val = "-1";
  }
  if(!pv_filter.isEmpty()){
    pv_filter = pv_filter + " AND ";
  }
  // Get the field parameters. For this, get the row 0 (If empty, fields info should be aviable)
  QSqlRecord rec = record(0);
  QSqlField field = rec.field(field_name);
  // Select the correct SQL operator according to data type.
  if((field.type() == QVariant::Int)||(field.type() == QVariant::Double)){
    pv_filter = pv_filter + field_name + "=" + val;
  }else if(field.type() == QVariant::String){
    pv_filter = pv_filter + field_name + "='" + val + "' ";
  }else{
    std::cerr << "db_relational_model::" << __FUNCTION__ << ": unknow data type in field. Filter not added" << std::endl;
    return false;
  }
  //std::cout << "db_relational_model::" << __FUNCTION__ << "-> Filter: " << pv_filter.toStdString() << std::endl;
  setFilter(pv_filter);
  return true;
}

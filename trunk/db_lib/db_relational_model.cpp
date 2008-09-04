/***************************************************************************
 *   Copyright (C) 2008 by Philippe                                        *
 *   nel230@gmail.com                                                      *
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

/// NOTE: when user enter more charachter than the field can contain, data will be simply lost.
/// Must find somthing display warning before commit !

#include "db_relational_model.h"
#include <iostream>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlField>
#include <QSqlIndex>
#include <QModelIndex>
#include <QMessageBox>

db_relational_model::db_relational_model(const db_connection *cnn, const QString &table_name, QObject * parent)
  : QSqlRelationalTableModel(parent, cnn->get_db())
{
  pv_field_is_auto_value = 0;
  pv_field_is_required = 0;
  pv_field_is_read_only = 0;
  pv_child_model = 0;
  pv_parent_model = 0;
  pv_current_index_is_valid = false;
  //pv_row_to_insert = -1;
  pv_message_dialogs_enabled = true;
  init(cnn, table_name);
  pv_parent_has_row = false;
}

db_relational_model::db_relational_model(QObject * parent, QSqlDatabase db)
  : QSqlRelationalTableModel(parent, db)
{
  pv_field_is_auto_value = 0;
  pv_field_is_required = 0;
  pv_field_is_read_only = 0;
  pv_message_dialogs_enabled = true;
  pv_child_model = 0;
  pv_parent_model = 0;
  pv_current_index_is_valid = false;
  //pv_row_to_insert = -1;
  pv_parent_has_row = false;
}

db_relational_model::~db_relational_model()
{
  if(pv_field_is_auto_value != 0){
    delete[] pv_field_is_auto_value;
  }
  if(pv_field_is_required != 0){
    delete[] pv_field_is_required;
  }
  if(pv_field_is_read_only != 0){
    delete[] pv_field_is_read_only;
  }
  // NOTE: test child model commited.
}

bool db_relational_model::init(const db_connection *cnn, const QString &table_name)
{
  QString str;
  int i = 0;
  // Init the labels
  if(!pv_label.init(cnn, table_name)){
    std::cerr << "db_relational_model::" << __FUNCTION__ << ": pv_label.init() failed" << std::endl;
    return false;
  }
  setTable(table_name);
  // By default, set the table name to user friendly name
  pv_user_table_name = table_name;
  // MySQL specifics
  str = "QMYSQL";
  if(cnn->get_driver_type() == str){
    pv_using_mysql = true;
    pv_mysql_specific = new db_mysql_specific(cnn);
    if(!pv_mysql_specific->set_table(table_name)){
      std::cerr << "db_relational_model::" << __FUNCTION__ << ": unable to init mysql_specific class" << std::endl;
    }
  }else{
    pv_using_mysql = false;
  }
  // Store fields info
  pv_field_is_auto_value = new bool[columnCount()];
  pv_field_is_required =  new bool[columnCount()];
  pv_field_is_read_only =  new bool[columnCount()];
  for(i=0; i<columnCount(); i++){
    pv_field_is_auto_value[i] = init_field_is_auto_value(i);
    if(record(0).field(i).requiredStatus()){
      pv_field_is_required[i] = true;
    }else{
      pv_field_is_required[i] = false;
    }
    pv_field_is_read_only[i] = record(0).field(i).isReadOnly();
  }
  connect(
    this, SIGNAL(beforeInsert(QSqlRecord&)),
    this, SLOT(before_insert(QSqlRecord&) )
  );
/*
  connect(
    this, SIGNAL(primeInsert (int, QSqlRecord&)),
    this, SLOT(prime_insert(int, QSqlRecord&))
  );
*/
  emit sig_select_called();
/*
/// tests
for(i=0; i<columnCount(); i++){
  std::cout << "Field: " << record(0).field(i).name().toStdString();
  if(field_is_auto_value(i)){
    std::cout << " AUTO ";
  }
  std::cout  << std::endl;
}
*/
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
  if(pv_child_model != 0){
    pv_child_model->set_user_headers();
  }
}

void db_relational_model::set_child_model(db_relational_model *model)
{
  pv_child_model = model;
  pv_child_model->set_parent_model(this);
}

void db_relational_model::set_parent_model(db_relational_model *model)
{
  pv_parent_model = model;
}

bool db_relational_model::set_relation(const db_relation &relation)
{
  if(pv_child_model == 0){
    std::cerr << "db_relational_model::" << __FUNCTION__ << ": child model is not set" << std::endl;
    return false;
  }
  pv_as_parent_relation_fields = relation.get_parent_relation_fields();
  //pv_child_model->pv_as_child_relation_fields = relation.get_child_relation_fields();
  pv_child_model->set_as_child_relation_fields(relation.get_child_relation_fields());
  return true;
}

void db_relational_model::set_as_child_relation_fields(QStringList fields)
{
  pv_as_child_relation_fields = fields;
}

void db_relational_model::message_dialogs_enabled(bool enable)
{
  pv_message_dialogs_enabled = enable;
}

void db_relational_model::current_row_changed(const QModelIndex &index)
{
/*
//  if(!submit()){
  if(isDirty(index)){
  std::cout << "Dirty!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    if((lastError().type() == QSqlError::ConnectionError)||(lastError().type() == QSqlError::TransactionError)){
      if(pv_message_dialogs_enabled){
        QString msg;
        msg = tr("Some data where not stored");
        msg += tr("\n\nReported error:\n");
        msg += lastError().text();
        QMessageBox::critical(0, pv_user_table_name, msg);
      }
      std::cerr << "db_table_widget::current_row_changed: " << lastError().text().toStdString() << std::endl;
      std::cerr << "db_table_widget::current_row_changed: Error typeNb: " << lastError().type() << std::endl;
      if(pv_row_to_insert != -1){
        revertRow(pv_row_to_insert);
        pv_row_to_insert = -1;
      }else{
        revert();
      }
      return;
    }
  }
*/
  pv_current_index_is_valid = index.isValid();
  if(pv_child_model != 0){
    child_data_commited();
    update_child_relations(index);
  }
}

bool db_relational_model::delete_row(int row)
{
  int ret = 0;
  QString msg;
  if(pv_child_model != 0){
    if(pv_child_model->rowCount() > 0){
      std::cout << "Child rows: " << pv_child_model->rowCount() << std::endl;
      if(pv_message_dialogs_enabled){
        msg = tr("Follow child table contains data:\n"); 
        msg += pv_child_model->get_user_table_name();
        msg += tr("\nDelete all ?");
        ret = QMessageBox::warning(0, pv_user_table_name, msg, QMessageBox::Ok | QMessageBox::Cancel);
        if(ret == QMessageBox::Cancel){
          //revertAll();
          return false;
        }else if(!delete_child_rows()){
          msg = tr("Child delete failed !");
          msg += tr("\n\nReported error:\n");
          msg += pv_child_model->lastError().text();
          QMessageBox::critical(0, pv_user_table_name, msg);
          return false;
        }
      }else{
        return false; // Dialog diseabled
      }
    }
  }
  if(!removeRow(row)){
    return false;
  }
  return true;
}

bool db_relational_model::delete_child_rows()
{
  int row_count=0;
  if(pv_child_model != 0){
    row_count = pv_child_model->rowCount();
    std::cout << "Child row to delete: " << row_count << std::endl;
    while(pv_child_model->has_rows()){
      if(!pv_child_model->delete_row(0)){
        std::cerr << "ERROR while deleting row  "  << std::endl;
        return false;
      }
      QModelIndex index = createIndex(0, 0);
      current_row_changed(index);
    }
  }
  return true;
}

bool db_relational_model::has_rows()
{
  if(rowCount() > 0){
    //std::cout << "Rows : " << rowCount() << std::endl;
    return true;
  }
  return false;
}

void db_relational_model::set_user_table_name(const QString &name)
{
  pv_user_table_name = name;
}

QString db_relational_model::get_user_table_name()
{
  return pv_user_table_name;
}

QList<QVariant> db_relational_model::get_PKs(QModelIndex & index)
{
  int i;
  QList<QVariant> PKs_list;
  QVariant var;

  if(index.isValid()){
    // get the list of primary keys
    for(i=0; i < primaryKey().count(); i++){
      var = primaryKey().field(i).name();
      PKs_list << var;
    }
  }
  return PKs_list;
}

void db_relational_model::clear_filter()
{
  pv_filter.clear();
  setFilter(pv_filter);
}

bool db_relational_model::add_filter(const QString &field_name, QString val)
{
  if(field_name.isEmpty()){
    std::cerr << "db_relational_model::" << __FUNCTION__ << ": Table: '" << tableName().toStdString() << 
        "': no field name given" << std::endl;
    return false;
  }
  if(val.isEmpty()){
    std::cerr << "db_relational_model::" << __FUNCTION__ << ": Table: '" << tableName().toStdString() << 
        "': no criteria value given" << std::endl;
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

void db_relational_model::update_relations(const QStringList &relations_values)
{
  emit sig_before_select();
  clear_filter();
  pv_as_child_relation_values = relations_values;
  // For each field, add criteria to the filter
  int i=0;
 for(i=0; i<pv_as_child_relation_values.count(); i++){
    //std::cout << "db_relational_model::" << __FUNCTION__ << ": field: " << pv_as_child_relation_fields.value(i).toStdString() << std::endl;
    if(!add_filter(pv_as_child_relation_fields.value(i), pv_as_child_relation_values.value(i))){
      std::cerr << "db_relational_model::" << __FUNCTION__ << ": Table: " << tableName().toStdString() << 
          ": relation criteria failed" << std::endl;
    }
  }
  select();
  emit sig_select_called();
  // We can have another child. So we act as parent of this other child
  // So, we select the first row, then take the index.
  if(pv_child_model != 0){
    QModelIndex index = createIndex(0, 0);
    update_child_relations(index);
  }
}

void db_relational_model::update_child_relations(const QModelIndex &index)
{
  QStringList relations_values;
  if(pv_child_model != 0){
    if(index.isValid()){
      // Data as parent
      QSqlRecord rec = record(index.row());
      int i=0;
      for(i=0; i<pv_as_parent_relation_fields.count(); i++){
        //std::cout << "Field: " << pv_as_parent_relation_fields.at(i).toStdString() << std::endl;
        relations_values << rec.value(pv_as_parent_relation_fields.at(i)).toString();
      }
    }else{
      int i=0;
      for(i=0; i<pv_as_parent_relation_fields.count(); i++){
        relations_values << "-1";
      }
    }
    pv_child_model->update_relations(relations_values);
  }
}

bool db_relational_model::parent_has_valid_index()
{
  return pv_parent_model->current_index_is_valid();
}

bool db_relational_model::has_child_model()
{
  if(pv_child_model != 0){
    return true;
  }
  return false;
}

bool db_relational_model::has_parent_model()
{
  if(pv_parent_model != 0){
    return true;
  }
  return false;
}

QModelIndex db_relational_model::create_index(int row, int column)
{
  return createIndex(row, column);
}

bool db_relational_model::current_index_is_valid()
{
  return pv_current_index_is_valid;
}

void db_relational_model::before_insert(QSqlRecord &rec)
{
  int i=0;
  //std::cout << "********* Before inser call..." << std::endl;
/*
  if(!pv_parent_has_row){
    revert();
    return;
  }
*/
  // data in record
  for(i=0; i<rec.count(); i++){
    std::cout << "*->>** Value: " << rec.value(i).toString().toStdString() << std::endl;
  }
  //std::cout << "db_table_widget::as_child_before_insert() Call.." << std::endl;
  for(i=0; i<columnCount(); i++){
    std::cout << rec.field(i).name().toStdString() << std::endl;
    if(field_is_auto_value(i)/*&&(!rec.field(i).isGenerated())*/){
      //std::cout << "********** Auto filed: " << rec.field(i).name().toStdString() << std::endl;
      if(!rec.field(i).isGenerated()){
        std::cout << rec.field(i).name().toStdString() << ": *Not generated" << std::endl;
      }else{
        //std::cout << rec.field(i).name().toStdString() << ": OK generated" << std::endl;
      }
    }
    if(field_is_read_only(i)){
      //std::cout << rec.field(i).name().toStdString() << ": Read Only" << std::endl;
    }
    if(field_is_required(i)){
      //std::cout << rec.field(i).name().toStdString() << ": Required" << std::endl;
    }
  }
  for(i=0; i<pv_as_child_relation_values.count(); i++){
    rec.setValue(pv_as_child_relation_fields.value(i), pv_as_child_relation_values.value(i));
  }
}

int db_relational_model::row_count()
{
  return rowCount();
}

QString db_relational_model::get_text_data(int row, int column)
{
  QModelIndex index = createIndex(row, column);
  current_row_changed(index);
  return data(index).toString();
}

db_relational_model *db_relational_model::get_child_model()
{
  return pv_child_model;
}

/*
void db_relational_model::prime_insert(int row, QSqlRecord & record)
{
  // I found nothing clean to handle correctly uncommited rows...
  // We store the row that schould be written. Then, it will be
  // used in current_row_changed() to know wich row is to revert
  pv_row_to_insert = row;
  std::cout << "db_relational_model::prime_insert: row to insert: " << row << std::endl;
}
*/

bool db_relational_model::init_field_is_auto_value(int col)
{
  if(pv_using_mysql){
    return pv_mysql_specific->is_auto_value(col);
  }else{
    return record(0).field(col).isAutoValue();
  }
  return false;
}

bool db_relational_model::field_is_auto_value(int col)
{
  if((col >= columnCount())||(pv_field_is_auto_value == 0)) {
    return false;
  }
  return pv_field_is_auto_value[col];
}

bool db_relational_model::field_is_required(int col)
{
  if((col >= columnCount())||(pv_field_is_required == 0)) {
    return false;
  }
  return pv_field_is_required[col];
}

bool db_relational_model::field_is_read_only(int col)
{
  if((col >= columnCount())||(pv_field_is_read_only == 0)){
    return false;
  }
  return pv_field_is_read_only[col];
}

bool db_relational_model::child_data_commited()
{
  int row = 0, col = 0;
  int ret = 0;
  QString msg;
  QModelIndex index;

  if(pv_child_model == 0){
    return true;
  }

  for(row=0; row < pv_child_model->rowCount(); row++){
    for(col=0; col < pv_child_model->columnCount(); col++){
      index = pv_child_model->create_index(row, col);
      if(pv_child_model->isDirty(index)){
        std::cout << "-->> Dirty!!!! index(" << row << ";" << col << ")" << std::endl;
        if(pv_message_dialogs_enabled){

          msg = tr("Follow child table contains data to save:\n"); 
          msg += pv_child_model->get_user_table_name();
          msg += tr("\nSave this records ?");
          ret = QMessageBox::warning(0, pv_user_table_name, msg, QMessageBox::Ok | QMessageBox::Cancel);
          if(ret == QMessageBox::Ok){
            pv_child_model->submitAll();
          }
        }else{
          return false;
        }
      }
    }
  }
  return true;
}

// Mysql specific class - Bug 194595 - Corrected in Qt 4.4.0

db_mysql_specific::db_mysql_specific(const db_connection *cnn)
{
  pv_mysql = 0;

  // Get running MYSQL handle
  QVariant driver = cnn->get_db().driver()->handle();
  if(strcmp(driver.typeName(), "MYSQL*")==0){
    pv_mysql = *static_cast<MYSQL **>(driver.data());
  }
  if(pv_mysql == 0){
    std::cerr << "db_mysql_specific::" << __FUNCTION__ << ": uable to get MySQL handle" << std::endl;
    pv_valid = false;
    pv_mysql = 0;
    return;
  }
  pv_valid = true;
}

db_mysql_specific::~db_mysql_specific()
{
}

bool db_mysql_specific::set_table(const QString &table_name)
{

  QString SQL;
  QByteArray b_SQL;
  const char *c_SQL;
  MYSQL_RES *result = 0;
  MYSQL_FIELD *field = 0;

  SQL = "SELECT * FROM " + table_name;
  b_SQL = SQL.toAscii();
  c_SQL = b_SQL.constData();

  if(!pv_valid){
    return false;
  }

  if((mysql_query(pv_mysql, c_SQL)) != 0){
    std::cerr << "db_mysql_specific::" << __FUNCTION__ << ": query failed" << std::endl;
    std::cerr << "db_mysql_specific::" << __FUNCTION__ << ": Error: " << mysql_error(pv_mysql) << std::endl;
    pv_valid = false;
    return false;
  }
  result = mysql_use_result(pv_mysql);
  if(result == 0){
    std::cerr << "db_mysql_specific::" << __FUNCTION__ << ": no reult" << std::endl;
    std::cerr << "db_mysql_specific::" << __FUNCTION__ << ": Error: " << mysql_error(pv_mysql) << std::endl;
    pv_valid = false;
    return false;
  }
  while((field = mysql_fetch_field(result))){
    if(field->flags & AUTO_INCREMENT_FLAG){
      pv_field_is_auto.append(true);
    }else{
      pv_field_is_auto.append(false);
    }
  }
  mysql_free_result(result);

  return true;
}

bool db_mysql_specific::is_auto_value(int col)
{
  if(!pv_valid){
    return false;
  }
  return pv_field_is_auto.at(col);
}

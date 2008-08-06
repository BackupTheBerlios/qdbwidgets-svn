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

#include "db_table_widget.h"
#include <iostream>
#include <QString>
#include <QStringList>
#include <QTableView>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSqlIndex>
#include <QSqlField>
#include <QList>
#include <QVariant>
#include <QPushButton>
#include <QMessageBox>
#include <QPoint>
#include <QItemSelectionModel>

db_table_widget::db_table_widget(const QString &name, QWidget *parent)
    : QWidget(parent)
{
  pv_table_model = 0;
  pv_table_view = 0;
}

bool db_table_widget::init(const db_connection *cnn, const QString &table_name)
{
  pv_layout = new QVBoxLayout(this);
  pv_hlayout = new QHBoxLayout(this);

  setWindowTitle(table_name);

  // Init the model
  pv_table_model = new db_relational_model(this, cnn->get_db());
  pv_table_model->init(cnn, table_name);
  pv_table_model->set_user_headers();

  //pv_FKs = new QSqlIndex();

  // Init the view
  pv_table_view = new QTableView(this);
  pv_table_view->setModel(pv_table_model);
  pv_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
  pv_table_view->setSelectionBehavior(QAbstractItemView::SelectItems);
  pv_table_view->resizeColumnsToContents();
  pv_table_view->setSortingEnabled(true);
  pv_table_view->sortByColumn(0, Qt::AscendingOrder);

  pv_layout->addWidget(pv_table_view);

  pb_insert = new QPushButton(tr("Insert"));
  pb_delete = new QPushButton(tr("Delete"));
  pv_hlayout->addWidget(pb_insert);
  pv_hlayout->addWidget(pb_delete);

  pv_layout->addLayout(pv_hlayout);

  connect(pb_insert, SIGNAL(clicked()), this, SLOT(insert_record()));
  connect(pb_delete, SIGNAL(clicked()), this, SLOT(delete_record()));
  // Signal from tableView whenn seleted (to update child)
  connect( pv_table_view->selectionModel(),
    SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
    this, SLOT(current_data_changed(const QModelIndex&)) );
  connect(
    pv_table_model, SIGNAL(beforeInsert(QSqlRecord&)),
    this, SLOT(as_child_before_insert(QSqlRecord&) )
  );

  return true;
}

void db_table_widget::set_selection_model(QItemSelectionModel *model)
{
  pv_table_view->setSelectionModel(model);
}

QAbstractItemModel * db_table_widget::get_model()
{
  return pv_table_view->model();
}

void db_table_widget::insert_record()
{
  int row = 0;
  pv_table_model->insertRows(row, 1);
}

void db_table_widget::delete_record()
{
  QModelIndex index = pv_table_view->currentIndex();
  if(index.isValid()){
    QMessageBox msgbox(QMessageBox::Warning, tr("Delete one row"), tr("Do you really want to delete selected item ?"), QMessageBox::Ok | QMessageBox::Cancel);
    int rep = msgbox.exec();
    if(rep == QMessageBox::Ok){
      int row = index.row();
      if(!pv_table_model->removeRow(row)){
        QMessageBox err_box(QMessageBox::Critical, tr("Delete failed"), tr("Delete failed"), QMessageBox::Ok);
      }
    }
  }
}

// Recieve from TableView, and emit to slot_current_data_changed - AS parent
void db_table_widget::current_data_changed(const QModelIndex &index)
{
  if(index.isValid()){
    // Data as parent
    QSqlRecord rec = pv_table_model->record(index.row());
    QStringList relations_values;
    int i=0;
    for(i=0; i<pv_as_parent_relation_fields.count(); i++){
      relations_values << rec.value(i).toString();
    }
    // Emit to child
    emit sig_current_data_changed(relations_values);
  }else{
    QStringList relations_values;
    int i=0;
    for(i=0; i<pv_as_parent_relation_fields.count(); i++){
      relations_values << "-1";
    }
    // Emit to child
    emit sig_current_data_changed(relations_values);
  }
}

void db_table_widget::as_child_before_insert(QSqlRecord &rec)
{
  int i=0;
  for(i=0; i<pv_as_child_relation_values.count(); i++){
    rec.setValue(pv_as_child_relation_fields.value(i), pv_as_child_relation_values.value(i));
  }
}

// AS child, recieve data_changed signal FROM parent
void db_table_widget::slot_current_data_changed(const QStringList &relations_values)
{
  pv_table_model->clear_filter();
  pv_as_child_relation_values = relations_values;
  // For each field, add criteria to the filter
  int i=0;
  for(i=0; i<pv_as_child_relation_values.count(); i++){
    if(!pv_table_model->add_filter(pv_as_child_relation_fields.value(i), pv_as_child_relation_values.value(i))){
      std::cerr << "db_table_widget::" << __FUNCTION__ << ": relation criteria failed" << std::endl;
    }
  }
  select();
  // We can have another child. So we act as parent of this other child
  // So, we select the first row, then take the index. If valid, emit signal
  QModelIndex index = pv_table_view->indexAt(QPoint(0,0));
  emit current_data_changed(index);
}

void db_table_widget::select()
{
  if(!pv_table_model->select()){
    std::cerr << "db_table_widget::" << __FUNCTION__ << ": select() failed" << std::endl;
    std::cerr << pv_table_model->lastError().text().toStdString().c_str() << std::endl;
  }
}

void db_table_widget::set_editable(bool editable)
{
  if(editable == true){
    pv_table_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
  }else{
    pv_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
  }
}

int db_table_widget::field_count()
{
  return pv_table_model->columnCount();
}

QString db_table_widget::get_table_name()
{
  return pv_table_model->tableName();
}

QSqlRecord db_table_widget::get_record(int row)
{
  return pv_table_model->record(row);
}

QStringList db_table_widget::get_header_data()
{
  QStringList headers;
  int i;
  for(i=0; i<pv_table_model->columnCount() ;i++){
    headers << pv_table_model->headerData(i , Qt::Horizontal).toString();
  }
  return headers;
}

QList<QVariant> db_table_widget::get_PKs()
{
  QModelIndex index;
  index = pv_table_view->currentIndex();
  int i;
  QList<QVariant> PKs_list;
  QVariant var;
  // get the list of primary keys
  for(i=0; i<pv_table_model->primaryKey().count(); i++){
    var = pv_table_model->primaryKey().field(i).name();
    PKs_list << var;
  }
  return PKs_list;
}

void db_table_widget::add_as_parent_relation_field(const QString &field_name)
{
  pv_as_parent_relation_fields << field_name;
}

void db_table_widget::add_as_child_relation_field(const QString &field_name)
{
  pv_as_child_relation_fields << field_name;
}

void db_table_widget::hide_field(const QString &field_name)
{
  int index = pv_table_model->fieldIndex(field_name);
  pv_table_view->setColumnHidden(index, true);
}

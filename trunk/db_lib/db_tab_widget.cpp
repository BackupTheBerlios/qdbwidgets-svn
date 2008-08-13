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

#include "db_tab_widget.h"
#include <iostream>
#include <QString>
#include <QSqlError>
#include <QSqlField>
#include <QVariant>
#include <QGridLayout>
#include <QSqlRecord>
#include <QPushButton>
#include <QSqlIndex>
#include <QModelIndex>
#include <QMessageBox>
#include <QItemSelectionModel>

/* Tab widget impl */

db_tab_widget::db_tab_widget(const QString &name, QWidget *parent)
    : QWidget(parent)
{
  pv_table_model = 0;

  // Init layout and arranges labels and texts
  pv_layout = new QGridLayout(this);

  lb_user_table_name = new QLabel;
  pv_layout->addWidget(lb_user_table_name);

  setWindowTitle(name);
}

bool db_tab_widget::set_model(db_relational_model *model)
{
  // Init the model
  pv_table_model = model;
  pv_table_model->set_user_headers();
  pv_table_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  lb_user_table_name->setText(pv_table_model->get_user_table_name());

  // Init the view
  pv_tab_view = new db_tab_view(this);
  pv_tab_view->setModel(pv_table_model);
  pv_tab_view->display_nav();
  pv_tab_view->set_auto_submit(false);

  pv_layout->addWidget(pv_tab_view);

  // Signal from tab_view whenn selected
  connect( pv_tab_view,
    SIGNAL(current_row_changed(int)),
    this, SLOT(current_row_changed(int)) );

  connect( this,
    SIGNAL(sig_current_row_changed(const QModelIndex&)),
    pv_table_model, SLOT(current_row_changed(const QModelIndex&)) );

  //connect( 
/*
  connect(myTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
         mapper, SLOT(setCurrentModelIndex(QModelIndex)));
*/
/*
  connect(
    pv_table_model, SIGNAL(beforeInsert(QSqlRecord&)),
    this, SLOT(as_child_before_insert(QSqlRecord&) )
  );
*/
  connect( pv_tab_view, SIGNAL(sig_delete_row(int)), this, SLOT(delete_record(int)));
  connect( pv_tab_view, SIGNAL(sig_insert_row()), this, SLOT(insert_record()));
  connect( pv_tab_view, SIGNAL(sig_submit(int)), this, SLOT(submit_all(int)));
  connect( pv_tab_view, SIGNAL(sig_revert()), this, SLOT(revert_all()));

  pv_table_model->setSort(0, Qt::AscendingOrder);
  select();

  return true;
}

void db_tab_widget::set_selection_model(QItemSelectionModel *model)
{
  pv_tab_view->setSelectionModel(model);
}

QAbstractItemModel * db_tab_widget::get_model()
{
  return pv_tab_view->model();
}

void db_tab_widget::select()
{
  if(!pv_table_model->select()){
    std::cerr << "db_table_widget::" << __FUNCTION__ << ": select() failed" << std::endl;
    std::cerr << pv_table_model->lastError().text().toStdString().c_str() << std::endl;
  }
  pv_tab_view->goto_first();
}

int db_tab_widget::field_count()
{
  return pv_table_model->columnCount();
}
/*
// Recieve from tab_view, and emit to slot_current_data_changed - AS parent
void db_tab_widget::current_data_changed(int row)
{
  if(row >= 0){
    // Data as parent
    QSqlRecord rec = pv_table_model->record(row);
    QStringList relations_values;
    int i=0;
    for(i=0; i<pv_as_parent_relation_fields.count(); i++){
      std::cout << "Field: " << pv_as_parent_relation_fields.at(i).toStdString() << std::endl;
      relations_values << rec.value(pv_as_parent_relation_fields.at(i)).toString();
    }
    // Emit to child
    emit sig_current_data_changed(relations_values);
  }else{
    QStringList relations_values;
    int i=0;
    for(i=0; i<pv_as_parent_relation_fields.count(); i++){
      std::cout << "Field: " << pv_as_parent_relation_fields.at(i).toStdString() << std::endl;
      relations_values << "-1";
    }
    // Emit to child
    emit sig_current_data_changed(relations_values);
  }
}
*/
void db_tab_widget::current_row_changed(int row)
{
  QModelIndex index = pv_table_model->create_index(row, 0);
  emit sig_current_row_changed(index);
}
/*
void db_tab_widget::as_child_before_insert(QSqlRecord &rec)
{
  std::cout << "db_TAB_widget::as_child_before_insert() Call.." << std::endl;
  int i=0;
  for(i=0; i<pv_as_child_relation_values.count(); i++){
    rec.setValue(pv_as_child_relation_fields.value(i), pv_as_child_relation_values.value(i));
  }
}
*/
bool db_tab_widget::submit_all(int current_row)
{
  QString msg;
  if(!pv_table_model->submitAll()){
    msg = tr("Unable to save data\n");
    msg += pv_table_model->lastError().text();
    QMessageBox::critical(this, tr("TITRE"), msg);
    return false;
  }
  select();
  pv_tab_view->goto_row(current_row);
  return true;
}

void db_tab_widget::revert_all()
{
  pv_table_model->revertAll();
}

QString db_tab_widget::get_table_name()
{
  return pv_table_model->tableName();
}

QSqlRecord db_tab_widget::get_record(int row)
{
  return pv_table_model->record(row);
}

QStringList db_tab_widget::get_header_data()
{
  QStringList headers;
  int i;
  for(i=0; i<pv_table_model->columnCount() ;i++){
    headers << pv_table_model->headerData(i , Qt::Horizontal).toString();
  }
  return headers;
}

void db_tab_widget::set_editable(bool editable)
{
  pv_is_editable = editable;
}

void db_tab_widget::hide_field(const QString &field_name)
{
  int index = pv_table_model->fieldIndex(field_name);
  if(index>0){
    //pv_entry[index].hide_field(true);
  }
}

void db_tab_widget::delete_record(int row)
{
  QString msg;
  if(!pv_table_model->delete_row(row)){
        msg = tr("Delete failed\n\nReported error:\n");
        msg += pv_table_model->lastError().text();
        QMessageBox err_box(QMessageBox::Critical, pv_table_model->get_user_table_name(), msg, QMessageBox::Ok);
        err_box.exec();
  }
  if(!pv_table_model->submitAll()){
        msg = tr("Delete failed\n\nReported error:\n");
        msg += pv_table_model->lastError().text();
        QMessageBox err_box(QMessageBox::Critical, pv_table_model->get_user_table_name(), msg, QMessageBox::Ok);
        err_box.exec();
  }
  pv_tab_view->goto_first();
  pv_tab_view->goto_row(row - 1);
}

/*
  Little problem here:
  QWidgetMapper will display the "last" data
  whenn the data are null. So, set empty data..
*/
void db_tab_widget::insert_record()
{
  int row = pv_table_model->rowCount();

  pv_table_model->insertRows(row, 1);
  pv_tab_view->goto_row(row);
  pv_tab_view->clear_displayed_data();
}

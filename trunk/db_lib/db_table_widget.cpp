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
#include <QLabel>

db_table_widget::db_table_widget(const QString &name, QWidget *parent)
    : QWidget(parent)
{
  pv_table_model = 0;
  pv_table_view = 0;

  pv_layout = new QVBoxLayout(this);
  pv_hlayout = new QHBoxLayout(this);

  lb_user_table_name = new QLabel;
  pv_layout->addWidget(lb_user_table_name);

  setWindowTitle(name);
}

bool db_table_widget::set_model(db_relational_model *model)
{
  pv_table_model = model;
  pv_table_model->set_user_headers();
  lb_user_table_name->setText(pv_table_model->get_user_table_name());
  // Init the view
  pv_table_view = new QTableView(this);
  pv_table_view->setModel(pv_table_model);
  pv_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
  pv_table_view->setSelectionBehavior(QAbstractItemView::SelectItems);
  pv_table_view->setSortingEnabled(true);
  pv_table_view->sortByColumn(0, Qt::AscendingOrder);
  pv_table_view->resizeColumnsToContents();
  pv_table_view->resizeRowsToContents();

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
    pv_table_model, SLOT(current_row_changed(const QModelIndex&)) );

  connect(pv_table_model,
    SIGNAL(modelReset()),
    this, SLOT(model_reset()) );

  connect(pv_table_model,
    SIGNAL(sig_select_called()),
    this, SLOT(model_reset()) );

  return true;
}

void db_table_widget::model_reset()
{
  pv_table_view->resizeRowsToContents();
}

void db_table_widget::set_selection_model(QItemSelectionModel *model)
{
  disconnect( pv_table_view->selectionModel(),
    SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
    pv_table_model, SLOT(current_row_changed(const QModelIndex&)) );

  pv_table_view->setSelectionModel(model);

  connect( pv_table_view->selectionModel(),
    SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
    pv_table_model, SLOT(current_row_changed(const QModelIndex&)) );
}

db_relational_model *db_table_widget::model()
{
  return pv_table_model;
}

void db_table_widget::insert_record()
{
  QString msg;
  int row = 0;
  if(pv_table_model->has_parent_model()){
    if(!pv_table_model->parent_has_row()){
      return;
    }
  }
  if(!pv_table_model->insertRow(row)){
    msg = tr("Insertion failed");
    QMessageBox msgbox(QMessageBox::Critical, pv_table_model->get_user_table_name(), msg);
  }
}

void db_table_widget::delete_record()
{
  QString msg;
  QModelIndex index = pv_table_view->currentIndex();
  if(index.isValid()){
    msg =  tr("Delete selected item ?");
    QMessageBox msgbox(QMessageBox::Warning, pv_table_model->get_user_table_name(), msg, QMessageBox::Ok | QMessageBox::Cancel);
    int rep = msgbox.exec();
    if(rep == QMessageBox::Ok){
      int row = index.row();
      if(!pv_table_model->delete_row(row)){
        msg = tr("Delete failed\n\nReported error:\n");
        msg += pv_table_model->lastError().text();
        QMessageBox err_box(QMessageBox::Critical, pv_table_model->get_user_table_name(), msg, QMessageBox::Ok);
        err_box.exec();
      }
    }
  }
}

void db_table_widget::select()
{
  if(!pv_table_model->select()){
    std::cerr << "db_table_widget::" << __FUNCTION__ << ": select() failed" << std::endl;
    std::cerr << pv_table_model->lastError().text().toStdString().c_str() << std::endl;
  }
  pv_table_view->resizeColumnsToContents();
  pv_table_view->resizeRowsToContents();
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

void db_table_widget::hide_field(const QString &field_name)
{
  int index = pv_table_model->fieldIndex(field_name);
  pv_table_view->setColumnHidden(index, true);
}

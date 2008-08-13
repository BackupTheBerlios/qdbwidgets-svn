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

#include "db_tab_view.h"
#include <iostream>
#include <QDataWidgetMapper>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QSqlRelationalDelegate>
#include <QPushButton>
#include <QModelIndex>
#include <QMessageBox>

db_tab_view::db_tab_view(QWidget *parent)
  : QWidget(parent)
{
  pb_first = 0;
  pb_previous = 0;
  pb_next = 0;
  pb_last = 0;
  pb_cancel = 0;
  pb_save = 0;
  pb_delete = 0;
  pb_insert = 0;
  lb_nb_rows = 0;
  pv_nav_layout = 0;
  pv_new_row = false;
  pv_mapper = new QDataWidgetMapper;
  pv_layout = new QGridLayout(this);
  setLayout(pv_layout);
  connect(pv_mapper, SIGNAL(currentIndexChanged(int)), this, SLOT(index_changed(int)));
  connect(this, SIGNAL(before_row_changed(int)), this, SLOT(detect_changes(int)));
}

void db_tab_view::bofore_model_select()
{
  detect_changes(pv_mapper->currentIndex());
  //goto_first();
}

void db_tab_view::model_selected()
{
  //std::cout << "db_tab_view::model_selected: rows: " << pv_mapper->model()->rowCount() << std::endl;
  if(lb_nb_rows != 0){
    QString txt = tr("Rows: ");
    QString row;
    //row.setNum(pv_mapper->currentIndex());
    //txt += row;
    //txt += tr(" of ");
    row.setNum(pv_mapper->model()->rowCount());
    txt += row;
    lb_nb_rows->setText(txt);
  }
  goto_row(0);
}

void db_tab_view::setModel(QAbstractItemModel *model)
{
  int col=0;
  unsetModel();
  pv_mapper->setModel(model);
  pv_mapper->setItemDelegate(new QSqlRelationalDelegate(pv_mapper));

  for(col=0; col < model->columnCount(); col++){
    pv_label_list.append(new QLabel);
    pv_label_list.at(col)->setText(model->headerData(col, Qt::Horizontal).toString());
    pv_layout->addWidget(pv_label_list.at(col));
    pv_edit_list.append(new QLineEdit);
    pv_layout->addWidget(pv_edit_list.at(col));
    pv_mapper->addMapping(pv_edit_list.at(col), col);
  }
  connect(model, SIGNAL(sig_before_select()), this, SLOT(bofore_model_select()) );
  connect(model, SIGNAL(sig_select_called()), this, SLOT(model_selected()) );
  //pv_mapper->toFirst();
}

void db_tab_view::unsetModel()
{
  while(!pv_label_list.isEmpty()){
    delete pv_label_list.takeFirst();
  }
  while(!pv_edit_list.isEmpty()){
    delete pv_edit_list.takeFirst();
  }
}

void db_tab_view::setSelectionModel(QItemSelectionModel *selectionModel)
{
  connect(selectionModel, SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), pv_mapper, SLOT(setCurrentModelIndex(QModelIndex)));
}

QAbstractItemModel * db_tab_view::model()
{
  return pv_mapper->model();
}

void db_tab_view::display_nav()
{
  if(pv_nav_layout == 0){
    pv_nav_layout = new QHBoxLayout(this);
    pv_layout->addLayout(pv_nav_layout, pv_layout->rowCount(), 0);
  }
  if(pb_first == 0){
    pb_first = new QPushButton(tr("<< First"));
    pv_nav_layout->addWidget(pb_first);
    connect(pb_first, SIGNAL(clicked()), this, SLOT(goto_first()));
  }
  if(pb_previous == 0){
    pb_previous = new QPushButton(tr("< Previous"));
    pv_nav_layout->addWidget(pb_previous);
    connect(pb_previous, SIGNAL(clicked()), this, SLOT(goto_previous()));
  }
  if(pb_next == 0){
    pb_next = new QPushButton(tr("Next >"));
    pv_nav_layout->addWidget(pb_next);
    connect(pb_next, SIGNAL(clicked()), this, SLOT(goto_next()));
  }
  if(pb_last == 0){
    pb_last = new QPushButton(tr("Last >>"));
    pv_nav_layout->addWidget(pb_last);
    connect(pb_last, SIGNAL(clicked()), this, SLOT(goto_last()));
  }
  if(pb_cancel == 0){
    pb_cancel = new QPushButton(tr("Cancel"));
    pv_nav_layout->addWidget(pb_cancel);
    connect(pb_cancel, SIGNAL(clicked()), this, SLOT(revert()));
  }
  if(pb_save == 0){
    pb_save = new QPushButton(tr("Save"));
    pv_nav_layout->addWidget(pb_save);
    connect(pb_save, SIGNAL(clicked()), this, SLOT(submit()));
  }
  if(pb_delete == 0){
    pb_delete = new QPushButton(tr("Delete"));
    pv_nav_layout->addWidget(pb_delete);
    connect(pb_delete, SIGNAL(clicked()), this, SLOT(delete_row()));
  }
  if(pb_insert == 0){
    pb_insert = new QPushButton(tr("Insert"));
    pv_nav_layout->addWidget(pb_insert);
    connect(pb_insert, SIGNAL(clicked()), this, SLOT(insert_row()));
  }
  if(lb_nb_rows == 0){
    lb_nb_rows = new QLabel(tr("Rows: "));
    pv_nav_layout->addWidget(lb_nb_rows);
  }
}

void db_tab_view::set_auto_submit(bool auto_submit)
{
  if(auto_submit){
    pv_mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
  }else{
    pv_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
  }
}

void db_tab_view::goto_row(int row)
{
  if(pv_mapper->model()->index(row, 0).isValid()) {
    pv_mapper->setCurrentIndex(row);
    set_all_editable(true);
  }else{
    std::cerr << "db_tab_view::goto_row: invalid index in model" << std::endl;
    set_all_editable(false);
    clear_displayed_data();
  }
}

void db_tab_view::clear_displayed_data()
{
  int i=0;
  for(i=0; i<pv_edit_list.count(); i++){
    pv_edit_list.at(i)->setText("");
  }
}

void db_tab_view::set_all_editable(bool editable)
{
  int i=0;
  for(i=0; i<pv_edit_list.count(); i++){
    if(editable){
      pv_edit_list.at(i)->setReadOnly(false);
    }else{
      pv_edit_list.at(i)->setReadOnly(true);
    }
  }
}

bool db_tab_view::submit()
{
  if(!pv_mapper->submit()){
    QMessageBox::critical(this, tr("TITRE"), tr("Unable to save data"));
    return false;
  }
  pv_new_row = false;
  emit sig_submit(pv_mapper->currentIndex());
  return true;
}

void db_tab_view::revert()
{
  pv_new_row = false;
  pv_mapper->revert();
  emit sig_revert();
}

void db_tab_view::index_changed(int row)
{
  if(pv_mapper->submitPolicy() == QDataWidgetMapper::ManualSubmit){
    //detect_changes(row);
  }
  emit current_row_changed(row);
}

void db_tab_view::goto_first()
{
  emit before_row_changed(pv_mapper->currentIndex());
  pv_mapper->toFirst();
}

void db_tab_view::goto_previous()
{
  emit before_row_changed(pv_mapper->currentIndex());
  pv_mapper->toPrevious();
}

void db_tab_view::goto_next()
{
  emit before_row_changed(pv_mapper->currentIndex());
  pv_mapper->toNext();
}

void db_tab_view::goto_last()
{
  emit before_row_changed(pv_mapper->currentIndex());
  pv_mapper->toLast();
}

void db_tab_view::detect_changes(int row)
{
  int i=0, ret;
  QString str1, str2;
  // Run only if the auto submit is unactive
  if(pv_mapper->submitPolicy() == QDataWidgetMapper::ManualSubmit){
    //std::cout << "Detect changes, row: " << row << std::endl;
    if(pv_new_row){
      // Just make diff
      str1 = "Null";
    }else{
      for(i=0; i<pv_edit_list.count(); i++){
        QModelIndex index = pv_mapper->model()->index(row, i);
        if(index.isValid()){
          str1 = pv_edit_list.at(i)->text();
          str2 = pv_mapper->model()->data(index).toString();
          //std::cout << "Detect changes, str1: " << str1.toStdString() << std::endl;
          //std::cout << "Detect changes, str2: " << str2.toStdString() << std::endl;
          if(str1 != str2){
            std::cout << "Detect changes: Diff" << std::endl;
            ret = QMessageBox::question(this, tr("Nom à mettre"), tr("Would you like to save changes ?"),
              QMessageBox::Save | QMessageBox::Cancel );
            if(ret == QMessageBox::Save){
              submit();
              return;
            }else{
              revert();
              return;
            }
          }
        }
      }
    }
  }
}

void db_tab_view::delete_row()
{
  int ret=0;
  ret = QMessageBox::warning(this, tr("Nom à mettre"), tr("Would you delete this row ?"),
            QMessageBox::Ok | QMessageBox::Cancel );
  if(ret == QMessageBox::Ok){
    emit sig_delete_row(pv_mapper->currentIndex());
  }
}

void db_tab_view::insert_row()
{
  pv_new_row = true;
  emit sig_insert_row();
}

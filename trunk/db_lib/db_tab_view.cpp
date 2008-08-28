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
#include "db_relational_model.h"
#include <iostream>
#include <QDataWidgetMapper>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QSqlRelationalDelegate>
#include <QPushButton>
#include <QModelIndex>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QUiLoader>
#include <QFile>

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
  pv_model = 0;
  pv_new_row = false;
  pv_text_edited = false;
  pv_mapper = new QDataWidgetMapper;
  // Main layout
  pv_vlayout = new QVBoxLayout(this);
  pv_glayout = new QGridLayout;
  lb_status = new QLabel(tr("Ready"));
  pv_vlayout->addWidget(lb_status);

  setLayout(pv_vlayout);
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
  goto_row(0);
}

void db_tab_view::setModel(db_relational_model *model)
{
  pv_model = model;
  pv_mapper->setModel(model);
  pv_mapper->setItemDelegate(new QSqlRelationalDelegate(pv_mapper));

  connect(model, SIGNAL(sig_before_select()), this, SLOT(bofore_model_select()) );
  connect(model, SIGNAL(sig_select_called()), this, SLOT(model_selected()) );
}

void db_tab_view::set_field_hidden(const QString &field_name)
{
  pv_hidden_fields.append(field_name);
}

bool db_tab_view::field_is_hidden(const QString &field_name)
{
  int index = pv_hidden_fields.indexOf(field_name);
  if(index < 0){
    return false;
  }
  return true;
}

bool db_tab_view::set_default_ui(label_position label_pos, int max_rows)
{
  int item = 0, row = 0, col = 0;
  int box_item = 0, box_col = 0; // Such a virtual box to store labels and edits
  int n_col = 0;  // Nb columns needed
  int fld_count = 0;

  if(pv_model == 0){
    std::cerr << "db_tab_view::" << __FUNCTION__ << ": model not set" << std::endl;
    return false;
  }
  unset_ui();

  // fileds in model that must be displayed
  fld_count = pv_model->columnCount() - pv_hidden_fields.count();
  if((max_rows > fld_count)||(max_rows == 0)){
    max_rows = fld_count;
  }
  // Columns needed
  n_col = fld_count / max_rows;

  // For ech field in model
  row = 0;
  for(item = 0; item < pv_model->columnCount(); item++){
    // Add labels and lineEdits
    pv_label_list.append(new QLabel);
    pv_label_list.at(item)->setText(pv_model->headerData(item, Qt::Horizontal).toString());
    pv_edit_list.append(new QLineEdit);
    pv_required_list.append(pv_model->field_is_required(item));
    pv_autoval_list.append(pv_model->field_is_auto_value(item));
    pv_mapper->addMapping(pv_edit_list.at(item), item);
    connect(pv_edit_list.at(item), SIGNAL(textEdited(const QString&)), this, SLOT(text_edited(const QString&)) );

    if(!field_is_hidden(pv_model->record(0).field(item).name())){
      // Set layout
      if(box_item >= max_rows){
        box_item = 0;
        box_col = box_col + 4;  // Adding space for strech
      }
      // label positions
      if(label_pos == db_tab_view::over){
        row = box_item * 2;
        col = box_col;
      }else if(label_pos == db_tab_view::left){
        row = box_item;
        col = box_col;
      }
      pv_glayout->addWidget(pv_label_list.at(item), row, col);
      //std::cout << "--> ** Add Label[" << item << "] at: row: " << row << " col: " << col << std::endl;

      // LineEdit positions
      if(label_pos == db_tab_view::over){
        row++;
      }else if(label_pos == db_tab_view::left){
        col = box_col + 2;
        // Add space
        pv_glayout->setColumnMinimumWidth(col+1, 20);
        //std::cout << "--> ** Add space at col: " << col+1 << std::endl;
      }
      pv_glayout->addWidget(pv_edit_list.at(item), row, col);
      //std::cout << "--> ** Add Edit[" << item << "] at: row: " << row << " col: " << col << std::endl;
      box_item++;
    }
  }
  pv_vlayout->addLayout(pv_glayout);
  return true;
}

bool db_tab_view::set_custom_ui(const QString &path)
{
  QUiLoader loader;
  QFile file(path);
  QWidget *custom_ui = 0;
  int item = 0;

  if(pv_model == 0){
    std::cerr << "db_tab_view::" << __FUNCTION__ << ": model not set" << std::endl;
    return false;
  }
  unset_ui();

  if(!file.open(QFile::ReadOnly)){
    std::cerr << "db_tab_view::" << __FUNCTION__ << ": unable to open file: " << path.toStdString() << std::endl;
    return false;
  }
  custom_ui = loader.load(&file);
  if(custom_ui == 0){
    std::cerr << "db_tab_view::" << __FUNCTION__ << ": unable to load UI : " << path.toStdString() << std::endl;
    return false;
  }
  pv_vlayout->addWidget(custom_ui);

  for(item = 0; item < pv_model->columnCount(); item++){
    QString field_name, lb_name, le_name;
    QLabel *tmp_lb = 0;
    QLineEdit *tmp_le = 0;
    field_name = pv_model->record(0).field(item).name();
    //std::cout << "-> filed(" << item << ") name: " << field_name.toStdString() << std::endl;
    lb_name = "lb_" + field_name;
    le_name = "le_" + field_name;

    // Find the label with compliant name
    tmp_lb = custom_ui->findChild<QLabel*>(lb_name);
    if(tmp_lb != 0){
      pv_label_list.append(tmp_lb);
      pv_label_list.at(item)->setText(pv_model->headerData(item, Qt::Horizontal).toString());
    }else{  // Not found, set a new label (not visible)
      pv_label_list.append(new QLabel);
    }
    // Find the lineEdit with compliant name
    tmp_le = custom_ui->findChild<QLineEdit*>(le_name);
    if(tmp_le != 0){
      pv_edit_list.append(tmp_le);
    }else{  // Not found, set a new lineEdit (not visible)
      pv_edit_list.append(new QLineEdit);
    }

    pv_required_list.append(pv_model->field_is_required(item));
    pv_autoval_list.append(pv_model->field_is_auto_value(item));
    pv_mapper->addMapping(pv_edit_list.at(item), item);
    connect(pv_edit_list.at(item), SIGNAL(textEdited(const QString&)), this, SLOT(text_edited(const QString&)) );

  }
  return true;
}

void db_tab_view::unset_ui()
{
  int i = 0;
  for(i=0; i<pv_edit_list.count(); i++){
    disconnect(pv_edit_list.at(i), SIGNAL(textEdited(const QString&)), this, SLOT(text_edited(const QString&)) );
    pv_mapper->removeMapping(pv_edit_list.at(i));
  }
  while(!pv_label_list.isEmpty()){
    delete pv_label_list.takeFirst();
  }
  while(!pv_edit_list.isEmpty()){
    delete pv_edit_list.takeFirst();
  }
  pv_required_list.clear();
  pv_autoval_list.clear();
}
/*
void db_tab_view::hide_field(const QString &filed_name)
{
  int index = 0;

  index = pv_model->record(0).indexOf(filed_name);
  if(index < 0){
    std::cerr << "db_tab_view::" << __FUNCTION__ << ": no field named: " << filed_name.toStdString() << std::endl;
    return;
  }
  // test if index is in pv_edit_list
  if(index >= pv_edit_list.size()){
    std::cerr << "db_tab_view::" << __FUNCTION__ << ": Error: index out of range" << std::endl;
    return;
  }
  // test if index is in pv_edit_list
  if(index >= pv_label_list.size()){
    std::cerr << "db_tab_view::" << __FUNCTION__ << ": Error: index out of range" << std::endl;
    return;
  }
  // Ok, found - hide them
  pv_edit_list.at(index)->hide();
  pv_label_list.at(index)->hide();
  //pv_glayout->removeWidget(pv_edit_list.at(index));
  //pv_glayout->removeWidget(pv_label_list.at(index));
}
*/
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
    pv_vlayout->addLayout(pv_nav_layout);
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
  int i=0;
  QString msg;
  bool ok = false;
  int index = 0;
  // Test if required fields have data
  for(i=0; i < pv_required_list.count(); i++){
    if((pv_required_list.at(i) == true)&&(pv_autoval_list.at(i) == false)&&(pv_edit_list.at(i)->text().isEmpty())){
      msg = pv_label_list.at(i)->text();
      msg += tr(":\nField is required");
      QMessageBox::warning(this, tr("TITRE"), msg);
      set_text_edited(false);
      return false;
    }
  }
/*
  //if((pv_mapper->submitPolicy() == QDataWidgetMapper::ManualSubmit)||(pv_new_row)){
    if(!pv_mapper->submit()){
    //if(!pv_model->submitAll()){
      msg = tr("Unable to save data\n\n");
      msg += tr("Reported error:\n");
      msg += pv_model->lastError().text();
      QMessageBox::critical(this, tr("TITRE"), msg);
      //revert();
      return false;
    }
    pv_new_row = false;
    set_text_edited(false);
    //emit sig_submit(pv_mapper->currentIndex());
  //}
*/

  // A problem with submitting simply call pv_mapper->submit()
  // An issue foundon: http://lists.trolltech.com/qt-interest/2007-05/thread00305-0.html
  index = pv_mapper->currentIndex();

  ok = pv_mapper->submit();
  if(ok){
    ok = pv_model->submitAll();
    if(ok){
      pv_mapper->setCurrentIndex(index);
      set_text_edited(false);
      pv_new_row = false;
    }else{
      msg = tr("Unable to save data to database ! (Datamodel submit failed)\n\n");
      msg += tr("Reported error:\n");
      msg += pv_model->lastError().text();
      QMessageBox::critical(this, tr("TITRE"), msg);
    }
  }else{
    msg = tr("Unable to save data to database !(Form submit failed)\n\n");
    msg += tr("Reported error:\n");
    msg += pv_model->lastError().text();
    QMessageBox::critical(this, tr("TITRE"), msg);
  }

/*
  ok = pv_model->submit();
  if(!ok){
    msg = tr("Unable to save data to database !\n\n");
    msg += tr("Reported error:\n");
    msg += pv_model->lastError().text();
    QMessageBox::critical(this, tr("TITRE"), msg);
  }
*/
  return true;
}

void db_tab_view::revert()
{
  //if(pv_mapper->submitPolicy() == QDataWidgetMapper::ManualSubmit){
    if(pv_new_row){
      pv_model->select();
      goto_first();
      set_text_edited(false);
    }
    pv_new_row = false;
    pv_mapper->revert();
    emit sig_revert();
  //}
}

void db_tab_view::index_changed(int row)
{
  if(pv_mapper->submitPolicy() == QDataWidgetMapper::ManualSubmit){
    //detect_changes(row);
  }
  emit current_row_changed(row);
  if(lb_nb_rows != 0){
    QString txt = tr("Rows: ");
    QString current_row, rows;
    current_row.setNum(row + 1);
    txt += current_row + "/";
    rows.setNum(pv_model->rowCount());
    txt += rows;
    lb_nb_rows->setText(txt);
  }
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
/*
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
*/

void db_tab_view::detect_changes(int row)
{
  int ret = 0;
  // Run only if the auto submit is unactive
  if(pv_mapper->submitPolicy() == QDataWidgetMapper::ManualSubmit){
    //std::cout << "Detect changes, row: " << row << std::endl;
/*
    if(pv_new_row){
      // Just say edited
      pv_text_edited = true;
    }else{
*/
        if(pv_text_edited){
          std::cout << "Detect changes: Diff" << std::endl;
          ret = QMessageBox::question(this, tr("Nom à mettre"), tr("Would you like to save changes ?"),
            QMessageBox::Save | QMessageBox::Cancel );
          if(ret == QMessageBox::Save){
            set_text_edited(true);
            submit();
            return;
          }else{
            set_text_edited(false);
            revert();
            return;
          }
        //}
      }
  }else{
    lb_status->setText(tr("Ready")); // Not clean !
  }
}

void db_tab_view::text_edited(const QString&)
{
  set_text_edited(true);
}

void db_tab_view::set_text_edited(bool edited)
{
  if(edited){
    lb_status->setText(tr("Row modified *"));
  }else{
    lb_status->setText(tr("Ready"));
  }
  pv_text_edited = edited;
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

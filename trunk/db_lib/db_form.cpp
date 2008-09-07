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

#include "db_form.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QPushButton>
#include <QTabWidget>
#include <iostream>

db_form::db_form(const QString &name, QWidget *parent)
    : QWidget(parent)
{
  pv_parent_view = 0;
}

bool db_form::init(const db_connection *cnn, const QString &parent_table_name/*, const QString &child_table_name*/)
{
  pv_vlayout = new QVBoxLayout(this);
  pv_tabs = new QTabWidget;

  pv_cnn = cnn;

  /// Parent model
  pv_model = new db_relational_model(pv_cnn, parent_table_name, this);
  if(!pv_model->is_valid()){
    return false;
  }
  pv_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

  /// relations (adresses)
  db_relation relation(parent_table_name);
  relation.add_parent_relation_field("soc_cod_pk");
  relation.set_child_table("adresse_societe_tbl");
  relation.add_child_relation_field("adr_soc_cod_fk");


  /// relations (contacts)
  db_relation con_relation(parent_table_name);
  con_relation.add_parent_relation_field("soc_cod_pk");
  con_relation.set_child_table("contact_societe_tbl");
  con_relation.add_child_relation_field("con_soc_cod_fk");

  /// relations (détails contacts)
  db_relation det_con_relation("contact_societe_tbl");
  det_con_relation.add_parent_relation_field("con_soc_id_pk");
  det_con_relation.set_child_table("detail_contact_societe_tbl");
  det_con_relation.add_child_relation_field("det_con_soc_id_fk");

  /// Parent view
  //pv_parent = new db_table_widget("test", this);
  pv_parent_view = new db_tab_widget("test", this);
  pv_vlayout->addWidget(pv_parent_view);
  pv_parent_view->set_model(pv_model);
  //pv_parent->set_field_hidden("id_cli_PK");
  //pv_parent->set_fields_hidden(relation.get_parent_relation_fields());
  pv_parent_view->set_default_ui(db_tab_view::left, 3);
  pv_parent_view->display_nav();

  /// Child
  add_child_table(relation);
  add_child_table(con_relation);
  add_child_table(det_con_relation);

  pv_vlayout->addWidget(pv_tabs);
  connect(pv_tabs, SIGNAL(currentChanged(int)), this, SLOT(tab_changed(int))  );
}

bool db_form::add_child_table(const db_relation &relation)
{
  int i = 0;
  bool sub_child = false;
  db_relational_model *parent_model = 0;
  QVBoxLayout *layout = 0;
  QWidget *new_widget = 0, *in_tab_widget = 0;

  // The parent could be in the childs list, 
  // the new child will then become a sub-child.
  // Fisrt, search in the childs list if one have the name
  // given as parent table in the relation. If non, the parent is pv_parent.
  for(i = 0; i < pv_child_models.size(); i++){
    if(pv_child_models.at(i)->tableName() == relation.get_parent_table()){
      // parent will be this child
      parent_model = pv_child_models.at(i);
    }
  }
  if(parent_model == 0){
    // Not found in childs, parent will be pv_parent
    parent_model = pv_model;
    sub_child = false;
  }else{
    sub_child = true;
  }
  // Create the new child model
  pv_child_models.append(new db_relational_model(pv_cnn, relation.get_child_table(),this));
  pv_child_models.last()->setEditStrategy(QSqlTableModel::OnRowChange);
  parent_model->add_child_model(pv_child_models.last());
  parent_model->set_relation(relation);
  // Create a new child view
  pv_child_views.append(new db_table_widget(pv_child_models.last()->tableName(), this));
  pv_child_views.last()->set_model(pv_child_models.last());
  // Layout
  new_widget = new QWidget;
  layout = new QVBoxLayout;

  if(sub_child){
    // Search in tabs wich widget has the objectName == given parent table
    for(i=0; i < pv_tabs->count(); i++){
      in_tab_widget = pv_tabs->widget(i);
      if(in_tab_widget->objectName() == relation.get_parent_table()){
        in_tab_widget->layout()->addWidget(pv_child_views.last());
      }
    }
  }else{
    layout->addWidget(pv_child_views.last());
    new_widget->setLayout(layout);
    new_widget->setObjectName(pv_child_models.last()->tableName());
    pv_tabs->addTab(new_widget , pv_child_models.last()->get_user_table_name());
  }

  return true;
}

void db_form::tab_changed(int index)
{
/*
  if(pv_tabs->currentWidget() != 0){
    db_table_widget *view = static_cast<db_table_widget*> (pv_tabs->currentWidget());
    db_relational_model *child_model = view->model();
    pv_model->set_child_model(child_model);
    //pv_model->select();
  }
*/
}

void db_form::select()
{
  pv_parent_view->select();
}

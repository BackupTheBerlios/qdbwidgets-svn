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
#include "db_relation.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QPushButton>
#include <iostream>

db_form::db_form(const QString &name, QWidget *parent)
    : QWidget(parent)
{
  pv_parent = 0;
  pv_child = 0;
}

bool db_form::init(const db_connection *cnn, const QString &parent_table_name, const QString &child_table_name)
{
  pv_vlayout = new QVBoxLayout(this);

  /// Models
  pv_model = new db_relational_model(cnn, "client_TBL",this);
  pv_child_model = new db_relational_model(cnn, "address_client",this);
  pv_sub_child_model = new db_relational_model(cnn, "detail_adresse_cli",this);

  pv_model->set_child_model(pv_child_model);
  pv_child_model->set_child_model(pv_sub_child_model);

  /// relation
  db_relation relation(parent_table_name);
  relation.add_parent_relation_field("id_cli_PK");
  relation.add_parent_relation_field("id_nom_PK");
  relation.add_child_relation_field("id_cli_FK");
  relation.add_child_relation_field("nom_cli_FK");
  pv_model->set_relation(relation);

  /// relation
  db_relation relation2("address_client");
  relation2.add_parent_relation_field("id_add_PK");
  relation2.add_child_relation_field("id_adresse_FK");
  pv_child_model->set_relation(relation2);

  //pv_parent = new db_table_widget("test", this);
  pv_parent = new db_tab_widget("test", this);
  pv_vlayout->addWidget(pv_parent);
  pv_parent->set_model(pv_model);
  //pv_parent->hide_field("id_cli_PK");

  pv_child = new db_table_widget("test", this);
  pv_vlayout->addWidget(pv_child);
  pv_child->set_model(pv_child_model);
  //pv_child->hide_field("id_cli_FK");
  //pv_child->hide_field("nom_cli_FK");

  pv_s_child = new db_table_widget("test", this);
  pv_vlayout->addWidget(pv_s_child);
  pv_s_child->set_model(pv_sub_child_model);

  pv_hlayout = new QHBoxLayout(this);
  pv_vlayout->addLayout(pv_hlayout);
/*
  /// Relation
  connect(
    pv_parent, SIGNAL(sig_current_data_changed(const QStringList&)),
    pv_child, SLOT(slot_current_data_changed(const QStringList&))
  );
  connect(
    pv_child, SIGNAL(sig_current_data_changed(const QStringList&)),
    pv_s_child, SLOT(slot_current_data_changed(const QStringList&))
  );
*/
  //select();
}

void db_form::select()
{
  pv_parent->select();
}

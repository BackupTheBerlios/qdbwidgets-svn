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

#include "main_dialog.h"

#include <iostream>

main_dialog::main_dialog(const QString & name, QWidget *parent)
  : QDialog(parent)
{
  //pv_form = new db_form(name, this);
  pv_tabw = 0;
  pv_tablew = 0;
}

bool main_dialog::init(const db_connection *cnn, const QString& table_name)
{
    pv_table_name = table_name;

    pv_hlayout = new QHBoxLayout(this);
    pb_tabw = new QPushButton(tr("Browse data"));
    pb_tablew = new QPushButton(tr("Data table"));
    pb_form = new QPushButton(tr("Form"));
    pb_searchw = new QPushButton(tr("Search"));
    pb_queryw = new QPushButton(tr("SQL query"));
    pb_systablew = new QPushButton(tr("Edit field names"));
    pb_tests = new QPushButton(tr("Tests"));
    pv_sql_cbox = new db_sql_combobox(tr("SQL list"));
    pv_hlayout->addWidget(pb_tabw);
    pv_hlayout->addWidget(pb_tablew);
    pv_hlayout->addWidget(pb_form);
    pv_hlayout->addWidget(pb_searchw);
    pv_hlayout->addWidget(pb_queryw);
    pv_hlayout->addWidget(pb_systablew);
    pv_hlayout->addWidget(pb_tests);
    pv_hlayout->addWidget(pv_sql_cbox);

    /// Tests
    pv_model = new db_relational_model(cnn, pv_table_name, 0);
    //pv_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    /// tests
    pv_child_model = new db_relational_model(cnn, "adresse_client_tbl",this);
    pv_model->add_child_model(pv_child_model);
  /// relation
  db_relation relation(pv_table_name);
  relation.add_parent_relation_field("cli_cod_pk");
  //relation.add_parent_relation_field("id_nom_PK");
  relation.add_child_relation_field("adr_cli_cod_fk");
  //relation.add_child_relation_field("nom_cli_FK");
  pv_model->set_relation(relation);


/*
    pv_tabw = new db_tab_widget("test");
    pv_tabw->set_model(pv_model);
    pv_tabw->set_field_hidden("id_cli_PK");
    pv_tabw->set_default_ui();
    pv_tabw->display_nav();
    //pv_tabw->init(cnn, pv_table_name)
    //pv_tabw->select();
*/
    /// TESTS avec selectionsmodeles
//    pv_selection_model = new QItemSelectionModel(pv_model);
//    pv_tablew->set_selection_model(pv_selection_model);
//    pv_tabw->set_selection_model(pv_selection_model);

    pv_form = new db_form("test");
    pv_form->init(cnn, pv_table_name/*, "address_client"*/);
    pv_searchw = new db_search_widget("test");
    pv_searchw->init(cnn, pv_table_name);

    pv_sys_model = new db_relational_model(cnn, "db_sys_field_name_TBL", 0);
    pv_systable = new db_table_widget("test");
    pv_systable->set_model(pv_sys_model);

    //pv_systable->init(cnn, "db_sys_field_name_TBL");
    pv_sql_cbox->init(cnn);
    pv_sql_cbox->new_query("SELECT id_cli_PK, nom_cli, remarques FROM client_TBL");
    pv_sql_cbox->set_data_field("id_cli_PK");
    //pv_sql_cbox->set_data_field("nom_cli");
    pv_sql_cbox->show_data_field(false);

    pv_queryw = new db_query_widget("test");
    pv_queryw->init(cnn, "SELECT * FROM " + pv_table_name + "; ");

    // Tests html
    pv_html = new db_html_engine();
    pv_html->init(cnn, table_name);

    connect(pb_tabw, SIGNAL(clicked()), this, SLOT(open_tab()));
    connect(pb_tablew, SIGNAL(clicked()), this, SLOT(open_table()));
    connect(pb_form, SIGNAL(clicked()), this, SLOT(open_form()));
    connect(pb_searchw, SIGNAL(clicked()), this, SLOT(open_search()));
    connect(pb_queryw, SIGNAL(clicked()), this, SLOT(open_query()));
    connect(pb_systablew, SIGNAL(clicked()), this, SLOT(open_systable()));
    connect(pb_tests, SIGNAL(clicked()), this, SLOT(tests()));

    connect(
      pv_sql_cbox, SIGNAL(sig_data(const QString&)), this, SLOT(display_text(const QString&))
    );

}

bool main_dialog::setup_table_view()
{
  if(pv_tablew == 0){
    pv_tablew = new db_table_widget("test");
    //pv_tablew->init(cnn, pv_table_name);
    pv_tablew->set_model(pv_model);
    pv_tablew->select();
    pv_tablew->show();
  }else{
    delete pv_tablew;
    pv_tablew = 0;
  }
  return true;
}

bool main_dialog::setup_tab_view()
{
  if(pv_tabw == 0){
    pv_tabw = new db_tab_widget("test");
    pv_tabw->set_model(pv_model);
    pv_tabw->set_field_hidden("id_cli_PK");
    pv_tabw->set_default_ui();
    pv_tabw->display_nav();
    pv_tabw->select();
    pv_tabw->show();
  }else{
    delete pv_tabw;
    pv_tabw = 0;
  }
  return true;
}

void main_dialog::display_text(const QString& txt)
{
  std::cout << "Selection: " << txt.toStdString().c_str() << std::endl;
}

void main_dialog::open_tab()
{
  int i=0;
  //for(i=0; i<1000; i++){
    setup_tab_view();
  //}
}

void main_dialog::open_search()
{
  pv_searchw->show();
}

void main_dialog::open_table()
{
  setup_table_view();
}

void main_dialog::open_form()
{
  pv_form->show();
  pv_form->select();
}

void main_dialog::open_query()
{
  pv_queryw->show();
}

void main_dialog::open_systable()
{
  pv_systable->show();
}

void main_dialog::tests()
{
  pv_html->tests();
}

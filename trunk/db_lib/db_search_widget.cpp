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

#include "db_search_widget.h"

#include <iostream>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QStringList>
#include <QPushButton>
#include <QSqlRecord>
#include <QSqlField>
#include <QLineEdit>
#include <QLabel>
#include <QRegExp>
#include <QSqlIndex>

db_search_widget::db_search_widget(const QString &name, QWidget *parent)
  : QWidget(parent)
{
  pv_search_table = 0;
  pv_name = name;
}

bool db_search_widget::init(const db_connection *cnn, const QString &table_name)
{
  // Table witch display results
  pv_data_model = new db_relational_model(cnn, table_name, this);
  pv_data_table = new db_table_widget(pv_name, this);
  pv_data_table->set_model(pv_data_model);
  pv_data_table->set_editable(false);

  setWindowTitle(table_name);

  // Tables for searches
  pv_search_table = new QTableWidget(3, pv_data_table->field_count(), this);
  // Set headers data
  QStringList headers;
  headers = pv_data_table->get_header_data();
  pv_search_table->setHorizontalHeaderLabels(headers);
  pv_search_table->resizeColumnsToContents();
  // Buttons
  pb_search = new QPushButton(tr("&Search"));

  // SQL line
  lb_sql = new QLabel;

  // Layout
  pv_vlayout = new QVBoxLayout(this);
  pv_hlayout = new QHBoxLayout(this);
  pv_vlayout->addWidget(pv_search_table);
  pv_vlayout->addWidget(pv_data_table);
  pv_hlayout->addWidget(pb_search);
  pv_vlayout->addWidget(lb_sql);
  pv_vlayout->addLayout(pv_hlayout);

  // Slots
  connect(pb_search, SIGNAL(clicked()), this, SLOT(search()));

  // TESTS
  //test = new db_table_widget(pv_name, this);
  //test->init(cnn, "address_client");
  //pv_vlayout->addWidget(test);
}

void db_search_widget::search()
{
  // FIXME: bugs in the filter construction with AND & OR
  QSqlRecord rec;
  rec = pv_data_table->get_record(0);
  QSqlField field;
  QString filter;
  QString SQL;
  SQL = "SELECT * FROM " + pv_data_table->get_table_name();
  // Watch every line
  int row;
  bool end_line = false;
  for(row=0; row<pv_search_table->rowCount(); row++){
    // Watch in every column:
    int i, y;
    end_line = false;
    y=0;
    for(i=0; i<pv_search_table->columnCount(); i++){
      QTableWidgetItem *item = 0;
      item = pv_search_table->item(row, i);
      if(item != 0){
        QString txt;
        txt = item->text();
        // Remove (FIXME end and beginning) white spaces
        txt.remove(QRegExp(" "));
        // Replace with correct string to the list
        item->setText(txt);
        if(!txt.isEmpty()){
          if((y>0)&&(end_line == false)){
            filter += " AND ";
          }
          if((row>0)&&(end_line == false)&&(!filter.isEmpty())){
            filter += " OR ";
          }
          field = rec.field(i);
          filter += field.name();
          // Search wich type of data we have, and assing correct operator
          if((field.type() == QVariant::Int)||(field.type() == QVariant::Double)){
            if((txt.at(0) != QChar('>'))&&(txt.at(0) != QChar('<'))){
              filter += " =";
            }
            filter += " ";
          }else if(field.type() == QVariant::String){
            filter += " LIKE '";
          }else{
            std::cerr << "db_search_widget::" << __FUNCTION__ << ": unknow data type" << std::endl;
          }
          filter += txt;
          // Search wich type of data we have, and assing correct operator
          if(field.type() == QVariant::Int){
            filter += " ";
          }else if(field.type() == QVariant::String){
            filter += "' ";
          }
          y++;
        }
      }
    }
    end_line = true;
  }
  std::cerr << "SQL filter: " << filter.toStdString().c_str() << std::endl;
  if(!filter.isEmpty()){
    SQL += " WHERE ";
  }
  SQL +=  filter;
  lb_sql->setText(SQL);
  pv_data_table->select();
}

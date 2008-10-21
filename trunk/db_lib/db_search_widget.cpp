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
  pv_search_table->resizeRowsToContents();
  // Buttons
  pb_search = new QPushButton(tr("&Search"));

  // Multi field search
  pb_multi_filed_search = new QPushButton(tr("Search in all fields"));
  le_search = new QLineEdit;

  // SQL line
  lb_sql = new QLabel;

  // Layout
  pv_vlayout = new QVBoxLayout(this);
  pv_hlayout = new QHBoxLayout(this);

  pv_hlayout->addWidget(le_search);
  pv_hlayout->addWidget(pb_multi_filed_search);
  pv_vlayout->addLayout(pv_hlayout);
  pv_vlayout->addWidget(pv_search_table);
  pv_vlayout->addWidget(pv_data_table);
  pv_vlayout->addWidget(lb_sql);
  pv_vlayout->addWidget(pb_search);

  // Slots
  connect(pb_search, SIGNAL(clicked()), this, SLOT(search()));
  connect(pb_multi_filed_search, SIGNAL(clicked()), this, SLOT(multi_filed_search()));

  // TESTS
  //test = new db_table_widget(pv_name, this);
  //test->init(cnn, "address_client");
  //pv_vlayout->addWidget(test);
}

void db_search_widget::multi_filed_search()
{
  QSqlRecord rec;
  QSqlField field;
  QString filter, txt;  // txt: searched string
  bool txt_is_num = false;
  QDate date; // for date format
  bool txt_is_date = false;
  QString SQL;
  int i=0, num_val = 0;

  if(le_search->text().isEmpty()){
    return;
  }

  rec = pv_data_model->record(0); // NOTE: empty test ?

  // Clear model's filter
  pv_data_model->setFilter(filter);

  SQL = "SELECT * FROM " + pv_data_table->get_table_name();

  // Get searched string
  txt = le_search->text();
  txt = remove_spaces(txt, true, true);
  le_search->setText(txt);
  // Test if is num value
  num_val = txt.toInt(&txt_is_num, 10);
  // Test if is date value
  date = get_date(txt);
  txt_is_date = date.isValid();

  // run into all fileds
  for(i=0; i<pv_data_model->columnCount(); i++){
    field = rec.field(i);
 
    // Search wich type of data we have, and assing correct operator
    if((field.type() == QVariant::Int)||(field.type() == QVariant::Double)){
      if(txt_is_num){
        if(i>0){
          filter += " OR ";
        }
        filter += field.name() + " =" + txt + " ";
      }
    }else if(field.type() == QVariant::Date){
      if(txt_is_date){
        if(i>0){
          filter += " OR ";
        }
        filter += field.name() + " ='" + date.toString("yyyy-MM-dd") + "' ";
      }
    }else if(field.type() == QVariant::String){
      if(i>0){
        filter += " OR ";
      }
      filter += field.name() + " LIKE '" + txt + "' ";
    }else{
      std::cerr << "db_search_widget::" << __FUNCTION__ << ": unknow data type" << std::endl;
      return;
    }
    //filter += txt;
  }
  std::cout << filter.toStdString() << std::endl;
  if(!filter.isEmpty()){
    SQL += " WHERE ";
  }
  SQL +=  filter;
  //lb_sql->setText(SQL);
  //lb_sql->setText(filter);
  pv_data_model->setFilter(filter);
  pv_data_table->select();
}

void db_search_widget::search()
{
  // FIXME: bugs in the filter construction with AND & OR
  QSqlRecord rec;
  rec = pv_data_table->get_record(0);
  QSqlField field;
  QString filter;
  QString SQL;
  bool txt_is_num = false;
  int num_val = 0;

  // Clear model's filter
  pv_data_model->setFilter(filter);

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
        // Remove end and beginning white spaces
        txt = remove_spaces(txt, true, true);
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
            }else if(txt.at(0) == QChar('>')){
              txt.remove(0, 1);
              filter += " >";
            }else if(txt.at(0) == QChar('<')){
              txt.remove(0, 1);
              filter += " <";
            }
            // Only numeric values are allowed here
            num_val = txt.toInt(&txt_is_num, 10);
            if(!txt_is_num){
              item->setText("#ERROR");
              return;
            }
          }else if(field.type() == QVariant::String){
            filter += " LIKE '";
          }else{
            std::cerr << "db_search_widget::" << __FUNCTION__ << ": unknow data type" << std::endl;
            return;
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
  std::cerr << "SQL filter: " << filter.toStdString() << std::endl;
  if(!filter.isEmpty()){
    SQL += " WHERE ";
  }
  SQL +=  filter;
  lb_sql->setText(SQL);
  //lb_sql->setText(filter);
  pv_data_model->setFilter(filter);
  pv_data_table->select();
  //pv_data_model->select();
}

QString db_search_widget::remove_spaces(QString str, bool at_beginning, bool at_end)
{
  int i = 0;
  // Kill whitespces at beginning
  if(at_beginning){
    for(i=0; i<str.size(); i++){
      if(str.at(i) == QChar(' ')){
        str.remove(i, 1);
      }else{
        break;
      }
    }
  }
  // Kill whitespces at end
  if(at_end){
    for(i=str.size()-1; i >= 0; i--){
      if(str.at(i) == QChar(' ')){
        str.remove(i, 1);
      }else{
        break;
      }
    }
  }
  return str;
}

QDate db_search_widget::get_date(QString str)
{
  QDate date;
  QStringList s_date;
  QString s_y, s_M, s_d;
  int i_y, i_M, i_d;
  bool ok = false;

  s_date = str.split(".");
  if(s_date.count() != 3){
    s_date = str.split("/");
  }
  if(s_date.count() != 3){
    return date;
  }
  // Try d/m/y formats
  s_d = s_date.at(0);
  s_M = s_date.at(1);
  s_y = s_date.at(2);

  // Test for num values
  i_d = s_d.toInt(&ok, 10);
  if(!ok){
    return date;
  }
  i_M = s_M.toInt(&ok, 10);
  if(!ok){
    return date;
  }
  i_y = s_y.toInt(&ok, 10);
  if(!ok){
    return date;
  }
  // If year was input is 1 digit
  if(s_y.length() == 1){
    i_y = i_y + 2000;
  }
  // If year was input 2 digit, like 08
  if((s_y.length() == 2)&&(s_y.at(0) == QChar('0'))){
    i_y = i_y + 2000;
  }

  date.setDate(i_y, i_M, i_d);

  //date.fromString(str, "dd.MM.yy");
  if(!date.isValid()){
    std::cerr << "DATE not valid " << std::endl;
  }
//  std::cout << "DATE: " << date.toString("yyyy-MM-dd").toStdString() << std::endl;
  return date;
}

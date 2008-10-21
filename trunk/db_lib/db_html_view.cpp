/***************************************************************************
 *   Copyright (C) 2008 by Philippe                                        *
 *   nel230@gmail.com                                                      *
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

#include "db_html_view.h"

#include <iostream>

#include <QTextDocument>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QSqlRecord>
#include <QSqlIndex>
#include <QSqlField>

db_html_view::db_html_view()
{
  pv_table_model = 0;
}

bool db_html_view::set_model(const db_relational_model *model)
{
  int i=0;
  pv_table_model = model;
  pv_table_model->set_user_headers();

  for(i=0; i<pv_table_model->columnCount(); i++){
    pv_headers.insert(i, pv_table_model->headerData(i, Qt::Horizontal).toString());
  }

  return true;
}

void db_html_view::set_attributes(db_html_view_attributes attributes)
{
  pv_attributes = attributes;
}

void db_html_view::set_field_hidden(const QString &field_name)
{
  int index;
  if(pv_table_model == 0){
    std::cerr << "db_html_view::" << __FUNCTION__ << ": pv_table_model not set" << std::endl;
    return;
  }
  index = pv_table_model->record(0).indexOf(field_name);
  //std::cout << "---------->  Hide field, name: " << field_name.toStdString() << " , index: " << index << std::endl;
  pv_hidden_fields.append(index);
}

void db_html_view::set_fields_hidden(const QStringList &fileds_name)
{
  int i=0;
  for(i=0; i<fileds_name.count(); i++){
    set_field_hidden(fileds_name.at(i));
  }
}

void db_html_view::set_pks_hidden()
{
  int i = 0;
  QSqlIndex index;

  if(pv_table_model == 0){
    std::cerr << "db_html_view::" << __FUNCTION__ << ": pv_table_model not set" << std::endl;
    return;
  }

  index = pv_table_model->primaryKey();
  for(i=0; i < index.count(); i++){
    set_field_hidden(index.field(i).name());
  }
}

bool db_html_view::field_is_hidden(int col)
{
  int index = pv_hidden_fields.indexOf(col);
  if(index < 0){
    return false;
  }
  return true;
}

QString db_html_view::get_user_table_name()
{
  QString html;

  html = "<table border=\"1\" style=\"margin-left: "
         + pv_attributes.get_left_margin() + "; " +
         " margin-top: " + pv_attributes.get_top_margin() + "; " +
         + "\" cellspacing=\"0\" cellpadding=\"3\">\n";
  html += "<tr>\n <td>";
  html += Qt::escape(pv_table_model->get_user_table_name());
  html += " </td>\n</tr>\n";
  html += " </table>\n";

  return html;
}

QString db_html_view::get_column_row(int row, db_relational_model *model)
{
  int i=0;
  QString html;

  html += "<tr>\n";
  for(i=0; i<pv_headers.count(); i++){
    html += get_data(row, i, model);
  }
  html += "\n</tr>\n";

  return html;
}

QString db_html_view::get_column_headers()
{
  int i=0;
  QString html;

  html += "<tr>\n";
  for(i=0; i<pv_headers.count(); i++){
    html += get_header(i);
  }
  html += "\n</tr>\n";

  return html;
}

QString db_html_view::get_column_view(bool with_headers, db_relational_model *model /*, QString html*/)
{

  //QString html;
/*
  html = "<table border=\"1\" style=\"margin-left: " + pv_attributes.get_left_margin() + ";\" cellspacing=\"0\" cellpadding=\"3\">\n";
  if(with_headers){
    html += get_column_headers();
  }
  // Write data
  for(i=0; i<pv_table_model->rowCount(); i++){
    QModelIndex index = pv_table_model->create_index(i, 0);
    html += get_column_row(i);

    if(model == 0){
      for(y=0; y < pv_table_model->child_models_count(); y++){
        get_column_view(with_headers, pv_table_model->get_child_model(y));
      }
    }else{
      for(z=0; z < model->rowCount(); z++){
        std::cout << "get data[" << i << ";" << y << ";" << z << "]" << std::endl;
        html += get_column_row(i, model);
      }
    }
    pv_table_model->current_row_changed(index);
  }

  html += "</table>\n";
*/
  test_rec(with_headers, model);

  return pv_html;
}

void db_html_view::test_rec(bool with_headers = true, db_relational_model *model)
{
  int i=0, y=0, z=0;

  if(model == 0){
    model = pv_table_model;
    std::cout << "->ROOT" << std::endl;
    pv_html = "";
  }

  /// Tests recursivité
  QString tbl_name;
  tbl_name = model->tableName();
  // data...
  pv_html += "<table border=\"1\">";
  for(i=0; i < model->rowCount(); i++){
    QModelIndex index = pv_table_model->create_index(i, 0);
    // For each column
    pv_html += "<tr>";
    std::cout << " --> Table: " << model->tableName().toStdString() << ": getting data[" << i << "]" << std::endl;
    for(y=0; y < model->columnCount(); y++){
      //std::cout << "----> Data... Table name: " << tbl_name.toStdString() << ": " << model->get_text_data(i, y).toStdString() << std::endl;
      pv_html += "<td>" + model->get_text_data(i, y) + "</td>";
      std::cout << model->get_text_data(i, y).toStdString() << " | ";
    }
    std::cout << std::endl;
    pv_html += "</tr>\n";
    model->current_row_changed(index);
    if(model->has_child_models()){
      // For each child model in same level
      for(y=0; y < model->child_models_count(); y++){
        std::cout << "  +--> Table: " << model->tableName().toStdString() << ": calling for child named: " << model->get_child_model(y)->tableName().toStdString() << std::endl;
        pv_html += "</table>\n";
        get_column_view(with_headers, model->get_child_model(y));
      }
    }
  }

  pv_html += "</table> RETURN\n";
  std::cout  << "-----> RETURN - TABLE: " << model->tableName().toStdString() << std::endl;
}

QString db_html_view::get_tab_row(int row, bool with_headers)
{
  int i=0;
  QString html;

  for(i=0; i<pv_headers.count(); i++){
    if(!field_is_hidden(i)){
      html += "<tr>\n";
      if(with_headers){
        html += get_header(i);
      }
      html += get_data(row, i);
      html += "\n</tr>\n";
    }
  }

  return html;
}

QString db_html_view::get_tab_view(bool with_headers)
{
  int i=0, y=0, headers_count = 0, rows_count = 0;
  QString html;

  headers_count = pv_headers.count();
  rows_count = pv_table_model->rowCount();

  //html = "<table border=\"0\" width=\"50%\" bordercolor=\"#000000\" cellspacing=\"0\">\n";
  html = "<table border=\"1\" style=\"margin-left: " + pv_attributes.get_left_margin() + ";\" cellspacing=\"0\" cellpadding=\"3\">\n";

  for(i=0; i<rows_count; i++){
    html += get_tab_row(i, with_headers);
  }
  html += "</table>\n";
  return html;
}

QString db_html_view::get_header(int column)
{
  QString html, txt;

  if(!field_is_hidden(column)){
    html = " <td>";
    html += Qt::escape(pv_headers.at(column));
    html += "</td>";
  }
  return html;
}

QString db_html_view::get_data(int row, int column, db_relational_model *model)
{
  int i = 0;
  QString html;

  if(model == 0){
    model = pv_table_model;
  }

  if(!field_is_hidden(column)){
    html = " <td>";
    html += Qt::escape(model->get_text_data(row, column));
    html += "</td>";
  }
  return html;
}

// db_html_view_attributes class

db_html_view_attributes::db_html_view_attributes()
{
}

void db_html_view_attributes::set_left_margin(int px)
{
  pv_left_margin.setNum(px);
  pv_left_margin += "px";
}

void db_html_view_attributes::set_top_margin(int px)
{
  pv_top_margin.setNum(px);
  pv_top_margin += "px";
}

QString db_html_view_attributes::get_left_margin()
{
  return pv_left_margin;
}

QString db_html_view_attributes::get_top_margin()
{
  return pv_top_margin;
}

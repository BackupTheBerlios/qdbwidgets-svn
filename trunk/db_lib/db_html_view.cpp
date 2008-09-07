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

QString db_html_view::get_column_row(int row)
{
  int i=0;
  QString html;

  html += "<tr>\n";
  for(i=0; i<pv_headers.count(); i++){
    html += get_data(row, i);
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

QString db_html_view::get_column_view(bool with_headers)
{
  int i=0;
  QString html;

  html = "<table border=\"1\" style=\"margin-left: " + pv_attributes.get_left_margin() + ";\" cellspacing=\"0\" cellpadding=\"3\">\n";

  if(with_headers){
    html += get_column_headers();
  }
  // Write data
  for(i=0; i<pv_table_model->rowCount(); i++){
    html += get_column_row(i);
  }

  html += "</table>\n";
  return html;
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

QString db_html_view::get_data(int row, int column)
{
  int i = 0;
  QString html;

  if(!field_is_hidden(column)){
    html = " <td>";
    html += Qt::escape(pv_table_model->get_text_data(row, column));
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

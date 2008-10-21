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

#ifndef DB_HTML_VIEW_H
#define DB_HTML_VIEW_H

#include "db_relational_model.h"

#include <QString>
#include <QStringList>

class QFile;
class QTextEdit;
class QTextDocument;

class db_html_view_attributes
{
 public:
  db_html_view_attributes();
  void set_left_margin(int px);
  void set_top_margin(int px);
  QString get_left_margin();
  QString get_top_margin();

 private:
  QString pv_left_margin;
  QString pv_top_margin;
};

class db_html_view
{
 public:
  db_html_view();
  /// Set hidden field
  void set_field_hidden(const QString &field_name);
  void set_fields_hidden(const QStringList &fileds_name);
  /// Set the primary keys hidden
  void set_pks_hidden();
  bool field_is_hidden(int col);
  bool set_model(const db_relational_model *model);
  void set_attributes(db_html_view_attributes attributes);
  QString get_user_table_name();
  QString get_column_row(int row, db_relational_model *model = 0);
  QString get_column_headers();
  QString get_column_view(bool with_headers = true, db_relational_model *model = 0 /*, QString html = QString()*/);
  QString get_tab_row(int row, bool with_headers = true);
  QString get_tab_view(bool with_headers = true);

 private:
  QString get_header(int column);
  QString get_data(int row, int column, db_relational_model *model = 0);
  db_relational_model *pv_table_model;
  QStringList pv_headers;
  QList<int> pv_hidden_fields;
  db_html_view_attributes pv_attributes;
  QString pv_html;
  void test_rec(bool with_headers = true, db_relational_model *model = 0);
};


#endif

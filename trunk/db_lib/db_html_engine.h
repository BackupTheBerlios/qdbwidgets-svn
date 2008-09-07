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

#ifndef DB_HTML_ENGINE_H
#define DB_HTML_ENGINE_H

#include "db_relational_model.h"
#include "db_connection.h"
#include "db_html_view.h"

#include <QString>
#include <QStringList>
#include <QList>


class QFile;
class QTextEdit;
class QTextBrowser;
class QTextDocument;

class db_html_engine
{
 public:
  db_html_engine();
  bool init(const db_connection *cnn, const QString &table_name);
  //QString get_column_view(int row = -1, bool with_headers = true, bool in_child = false);
  //QString get_tab_view(int row = -1, bool with_headers = true, bool in_child = false);
  //QString get_parent_child_view(int row = -1, bool with_headers = true);
  /// Add a child table. Give the relation, a list of hidden fields
  /// If pks_hidden is true, the primary keys of child will not be displayed
  bool add_child_table(const db_relation &relation, db_html_view_attributes attributes,
                        const QStringList hidden_fields = QStringList(), 
                        bool pks_hidden = false);
  QString get_data();
  void tests();

 private:
  //QString get_header(int column, bool in_child = false);
  //QString get_data(int row, int column, bool in_child = false);
  db_relational_model *pv_table_model;
  QList<db_relational_model*> pv_child_models;
  QList<db_html_view*> pv_child_views;
  db_connection *pv_cnn;
  db_html_view *pv_html_view;
  //db_relational_model *pv_child_model;
  QStringList pv_headers;
  //QStringList pv_child_headers;
  bool write_file(QString path, QString txt);
  // tests
  QTextEdit *pv_txt_edit;
  QTextBrowser *pv_txt_browser;
  QTextDocument *pv_doc;
};

#endif

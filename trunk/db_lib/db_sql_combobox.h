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

#ifndef DB_SQL_COMBOBOX_H
#define DB_SQL_COMBOBOX_H

#include <QComboBox>
#include <QItemDelegate>
#include <QTableView>
#include <QKeyEvent>
#include "db_connection.h"

class QString;
class QSqlQueryModel;
class QSqlRecord;

/* Delegate: to set a correct size to the combobox */
class db_sqlcb_item_delegate : public QItemDelegate
{
 Q_OBJECT
 public:
  db_sqlcb_item_delegate();
  ~db_sqlcb_item_delegate();
  QSize sizeHint (const QStyleOptionViewItem & option, const QModelIndex & index ) const;
  void set_height(int height);
  void set_widths(QSqlRecord record);

private:
  int pv_height;
  int pv_max_width;
  int *pv_widths;
};

/* The used view for combobex */
class db_sqlcb_view : public QTableView
{
 Q_OBJECT
 public:
  int sizeHintForRow (int row) const;
};

/* The sql combobox */
class db_sql_combobox : public QComboBox
{
 Q_OBJECT
 public:
  db_sql_combobox(const QString &name, QWidget *parent = 0);
  /// Call this init function before any use
  bool init(const db_connection *cnn);
  /// Set the SQL query
  bool new_query(const QString &SQL);
  /// Set the field witch contains the needed data
  bool set_data_field(const QString &field_name);
  /// show or not the data field
  void show_data_field(bool show);

 signals:
  /// Obtain data with this signal
  void sig_data(const QString &);

 private slots:
  void slot_data(int index);

 private:
    QSqlQueryModel *pv_query;    // Private instance of QSqlQuery
    const db_connection *pv_cnn;  // Connexion
    db_sqlcb_view *pv_view;
    db_sqlcb_item_delegate *pv_item_delegate;
    int pv_display_column;
    int pv_data_column;
    bool pv_show_data_column;
};

#endif

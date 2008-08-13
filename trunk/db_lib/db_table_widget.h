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

#ifndef DB_TABLE_WIDGET_H
#define DB_TABLE_WIDGET_H

#include "db_connection.h"
#include "db_relational_model.h"

#include <QWidget>

class QTableView;
class QSqlDatabase;
class QVBoxLayout;
class QHBoxLayout;
class QHeaderView;
class QSqlRecord;
class QModelIndex;
class QSqlIndex;
class QPushButton;
class QItemSelectionModel;
class QAbstractItemModel;
class QLabel;

class db_table_widget : public QWidget
{
 Q_OBJECT
  public:
    db_table_widget(const QString &name, QWidget *parent = 0);
    /// Set_model - Call this before any use
    bool set_model(db_relational_model *model);
    void set_selection_model(QItemSelectionModel *model);
    db_relational_model *model();
    /// Return number of fields (columns)
    int field_count();
    QString get_table_name();
    QSqlRecord get_record(int row);
    void select();
    QStringList get_header_data();
    void set_editable(bool editable);
    /// Hide a field
    void hide_field(const QString &field_name);

  signals:
    void sig_current_data_changed(const QStringList &relations_values);

  private slots:
    void insert_record();
    void delete_record();
    void model_reset();

  private:
    //QSqlRelationalTableModel *pv_table_model;
    db_relational_model *pv_table_model;
    QTableView *pv_table_view;
    QVBoxLayout *pv_layout;
    QHBoxLayout *pv_hlayout;
    QPushButton *pb_insert;
    QPushButton *pb_delete;
    QLabel *lb_user_table_name;
};

#endif

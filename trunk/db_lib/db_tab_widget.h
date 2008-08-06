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

#ifndef DB_TAB_WIDGET_H
#define DB_TAB_WIDGET_H

#include "db_connection.h"
#include "db_relational_model.h"
#include "db_tab_view.h"

#include <QWidget>

//class QSqlTableModel;
//class QSqlRelationalTableModel;
class QSqlDatabase;
class QGridLayout;
class QLineEdit;
class QLabel;
class QPushButton;
class QModelIndex;
class QItemSelectionModel;
class QAbstractItemModel;

class db_tab_widget : public QWidget
{
 Q_OBJECT
  public:
    db_tab_widget(const QString &name, QWidget *parent = 0);
    /// Call this init function before any use
    bool init(const db_connection *cnn, const QString &table_name);
    void set_selection_model(QItemSelectionModel *model);
    QAbstractItemModel * get_model();
    /// Return number of fields (columns)
    int field_count();
    /// Activate navigation buttons
    bool activate_nav();
    QString get_field_name(int col);
    QString get_table_name();
    QSqlRecord get_record(int row);
    void select();
    QStringList get_header_data();
    void set_editable(bool editable);
    /**
      Relation management:
        1: Define fileds in the relation, in the parent instance, and in child instance (order is important!)
        2: connect the signal sig_current_data_changed from parent to slot slot_current_data_changed in child
    **/
    /// add a filed to relation - AS parent form
    void add_as_parent_relation_field(const QString &field_name);
    /// add a filed to relation - AS child form
    void add_as_child_relation_field(const QString &field_name);
    /// Hide a field
    void hide_field(const QString &field_name);

  signals:
    void sig_current_data_changed(const QStringList &relations_values);

  private slots:
    void insert_record();
    void delete_record(int row);
    void current_data_changed(int row);
    // AS child instance, this slot recieve needed data for relations...
    void slot_current_data_changed(const QStringList &relations_values);
    void as_child_before_insert(QSqlRecord &rec);
    bool submit_all(int current_row);
    void revert_all();

  private:
    db_relational_model *pv_table_model;
    db_tab_view *pv_tab_view;
    QGridLayout *pv_layout;
    bool pv_is_editable;
    bool is_empty(QSqlRecord &rec);
    QString pv_filter_user_args;
    // List of fields "implqué" in relation, AS parent form
    QStringList pv_as_parent_relation_fields;
    // List of fields "implqué" in relation, AS child form
    QStringList pv_as_child_relation_fields;
    QStringList pv_as_child_relation_values;
};

#endif

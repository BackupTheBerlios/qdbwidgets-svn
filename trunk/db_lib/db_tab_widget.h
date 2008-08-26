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

/// Note: with this widget, the model's edit strategy should be set to: OnManualSubmit submit
/// Use: model->setEditStrategy(QSqlTableModel::OnManualSubmit); for this

class db_tab_widget : public QWidget
{
 Q_OBJECT
  public:
    db_tab_widget(const QString &name, QWidget *parent = 0);
    /// Set_model - Call this before any use
    bool set_model(db_relational_model *model);
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
    /// Hide a field
    void hide_field(const QString &field_name);

  signals:
    void sig_current_data_changed(const QStringList &relations_values);
    void sig_current_row_changed(const QModelIndex& index);

  private slots:
    void insert_record();
    void delete_record(int row);
    //void current_data_changed(int row);

    void current_row_changed(int row);
    //bool submit_all(int current_row);
    void revert_all();

  private:
    db_relational_model *pv_table_model;
    db_tab_view *pv_tab_view;
    QGridLayout *pv_layout;
    bool pv_is_editable;
    bool is_empty(QSqlRecord &rec);
    QString pv_filter_user_args;
    // List of fields "implqué" in relation, AS parent form
    //QStringList pv_as_parent_relation_fields;
    // List of fields "implqué" in relation, AS child form
    //QStringList pv_as_child_relation_fields;
    //QStringList pv_as_child_relation_values;
    QLabel *lb_user_table_name;
};

#endif

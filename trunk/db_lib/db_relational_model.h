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

#ifndef DB_RELATIONAL_MODEL
#define DB_RELATIONAL_MODEL

#include <QWidget>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QSqlRelationalTableModel>

#include "db_field_label.h"
#include "db_relation.h"

class QString;
class QStringList;
class QModelIndex;
class db_mysql_specific;

class db_relational_model : public QSqlRelationalTableModel
{
 Q_OBJECT

 public:
  /// Create the model for given connection and table name
  db_relational_model(const db_connection *cnn, const QString &table_name, QObject * parent = 0);
 /// If you use this contructor, don't forget to call init()
  db_relational_model(QObject * parent = 0, QSqlDatabase db = QSqlDatabase());
  ~db_relational_model();
  /// Call this init function if you use default constructor
  bool init(const db_connection *cnn, const QString &table_name);
  /// Test if model succesfull initialized (usefull if you don't use init() )
  bool is_valid();
  /// Set user friendly headers (found with db_field_label)
  void set_user_headers();
  /// Set given model the child model
  bool add_child_model(db_relational_model *model);
  // This is automatically called - don't use
  void set_parent_model(db_relational_model *model);
  /**
    Relation management:
      1: Use set_relation() to store relations fields (contained in a db_relation)
      2: connect the signal currentRowChanged() from selectionModel to slot current_row_changed()
  **/
  /// Set a db_relation containing needed data
  /// NOTE: the parent relation fields must allways be the same
  /// (there are the PK's of the parent table)
  bool set_relation(const db_relation &relation);
  // se
  void set_as_child_relation_fields(QStringList fields);
  /// if enabled, the model communicates error/ ask question directly (default: enabled, disabled: not tested!)
  void message_dialogs_enabled(bool enable);
  /// Helper function: checks child rows before delete
  bool delete_row(int row);
  // Should not directly be called
  bool delete_child_rows();
  /// Give the model a user friendly table name
  void set_user_table_name(const QString &name);
  QString get_user_table_name();

  void clear_filter();
  bool add_filter(const QString &field_name, QString val);
  // Get the list of Primary keys
  QList<QVariant> get_PKs(QModelIndex & index);
  // Called by parent model
  void update_relations(const QStringList &relations_values);
  // Return true while the model has rows
  bool has_rows();
  bool parent_has_valid_index();
  bool has_child_model();
  bool has_parent_model();
  bool current_index_is_valid();
  // Needed, because createIndex is protected
  QModelIndex create_index(int row, int column);
  int row_count();
  QString get_text_data(int row, int column);
  db_relational_model *get_child_model(const QString &table_name);
  /// get fields infos
  bool field_is_auto_value(int col);
  bool field_is_required(int col);
  bool field_is_read_only(int col);

 public slots:
  // Recieve row change signal from view (or view's selectionModel)
  void current_row_changed(const QModelIndex &index);
  // AS child, update relation list
  void before_insert(QSqlRecord &rec);
  //void prime_insert(int row, QSqlRecord & record);

 signals:
  void sig_before_select();
  void sig_select_called();

 private:
  db_field_label pv_label;          // Field labels
  QString pv_filter;
  // List of fields "implqué" in relation, AS parent form
  QStringList pv_as_parent_relation_fields;
  // List of fields "implqué" in relation, AS child form
  QStringList pv_as_child_relation_fields;
  QStringList pv_as_child_relation_values;
  bool pv_message_dialogs_enabled;
  QString pv_user_table_name; // User friendly table name
  //db_relational_model *pv_child_model;
  QList<db_relational_model*> pv_child_models;  // list of child models

  db_relational_model *pv_parent_model;
  void update_child_relations(const QModelIndex &index);
  //int pv_row_to_insert; // Look at before_insert() and current_row_changed()
  bool pv_parent_has_row; // When parent model contains no row, we must disable the add of row
  bool pv_using_mysql;
  db_mysql_specific *pv_mysql_specific;
  // Store field info
  bool *pv_field_is_auto_value;
  bool *pv_field_is_required;
  bool *pv_field_is_read_only;
  bool init_field_is_auto_value(int col);
  bool pv_current_index_is_valid;
  bool child_data_commited();
  bool pv_model_is_valid;
};

// Have a problem to test if a field is auot_increment with Mysql.
// Bug ?? At the moment, this class should help.
#ifdef WIN32
  #include <winsock.h>
#endif
#include <mysql/mysql.h>
class db_mysql_specific
{
 public:
  db_mysql_specific(const db_connection *cnn);
  ~db_mysql_specific();
  bool set_table(const QString &table_name);
  bool is_auto_value(int col);

 private:
  MYSQL *pv_mysql;
  bool pv_valid;
  QList<bool> pv_field_is_auto;
};

#endif // define DB_RELATIONAL_MODEL

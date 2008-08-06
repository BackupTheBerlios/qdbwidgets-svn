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

#ifndef DB_TAB_VIEW_H
#define DB_TAB_VIEW_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QList>
#include <QModelIndex>

class QDataWidgetMapper;
class QAbstractItemModel;
class QItemSelectionModel;
class QGridLayout;
class QHBoxLayout;
class QPushButton;

class db_tab_view : public QWidget
{
 Q_OBJECT

 public:
  db_tab_view(QWidget *parent = 0);
  void setModel(QAbstractItemModel *model);
  void setSelectionModel(QItemSelectionModel *selectionModel);
  QAbstractItemModel * model();
  void unsetModel();
  void display_nav();
  /// Set / unset Auto update of data
  void set_auto_submit(bool auto_update);
  void goto_row(int row);
  /// clear data in the line_edits.
  /// No update will be made in the model !
  void clear_displayed_data();
  bool revert();

 signals:
  void current_row_changed(int row);
  void before_row_changed(int row);
  void sig_delete_row(int row);
  void sig_insert_row();
  bool sig_submit(int current_row);
  void sig_revert();

 public slots:
  /// Submit data
  bool submit();
  void index_changed(int row);
  // Detect changes
  void detect_changes(int row);
  void goto_first();
  void goto_previous();
  void goto_next();
  void goto_last();
  void delete_row();
  void insert_row();

 private:
  QDataWidgetMapper *pv_mapper;
  QGridLayout *pv_layout;
  QList<QLineEdit*> pv_edit_list;
  QList<QLabel*> pv_label_list;
  QPushButton *pb_first;
  QPushButton *pb_previous;
  QPushButton *pb_next;
  QPushButton *pb_last;
  QPushButton *pb_save;
  QPushButton *pb_delete;
  QPushButton *pb_insert;
  QHBoxLayout *pv_nav_layout;
  bool pv_new_row;    // Please look at detect-changes() and insert_row()
};

#endif
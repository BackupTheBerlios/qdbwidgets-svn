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

#ifndef DB_FORM_H
#define DB_FORM_H

#include "db_table_widget.h"
#include "db_tab_widget.h"
#include "db_query_widget.h"
#include "db_relational_model.h"
#include "db_relation.h"

#include <QWidget>
#include <QString>
#include <QList>
#include <QVariant>
#include <QSqlDatabase>

class QModelIndex;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QTabWidget;

class db_form : public QWidget
{
 Q_OBJECT
  public:
    db_form(const QString &name, QWidget *parent = 0);
    bool init(const db_connection *cnn, const QString &parent_table_name/*, const QString &child_table_name*/);
    bool add_child_table(const db_relation &relation);
    void select();

  private slots:
    void tab_changed(int index);
  private:
    db_connection *pv_cnn;
    db_relational_model *pv_model;
    QList<db_relational_model*> pv_child_models;
    QList<db_table_widget*> pv_child_views;
    db_tab_widget *pv_parent_view;
    QVBoxLayout *pv_vlayout;
    QTabWidget *pv_tabs;
};

#endif

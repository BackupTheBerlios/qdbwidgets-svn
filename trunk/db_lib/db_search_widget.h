/***************************************************************************
 *   Copyright (C) 2008 by Philippe   *
 *   nel230@gmail.com   *
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

#ifndef DB_SEARCH_WIDGET_H
#define DB_SEARCH_WIDGET_H

#include "db_table_widget.h"
#include "db_relational_model.h"

#include <QWidget>
#include <QString>

class QTableWidget;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLineEdit;
class QLabel;

class db_search_widget : public QWidget
{
  Q_OBJECT
  public:
    db_search_widget(const QString &name, QWidget *parent = 0);
    bool init(const db_connection *cnn, const QString &table_name);

  private slots:
    void search();

  private:
    db_relational_model *pv_data_model;
    QVBoxLayout *pv_vlayout;
    QHBoxLayout *pv_hlayout;
    QTableWidget *pv_search_table;
    db_table_widget *pv_data_table;
    QString pv_name;
    QPushButton *pb_search;
    QLabel *lb_sql;

    // TESTS
    db_table_widget *test;
};

#endif

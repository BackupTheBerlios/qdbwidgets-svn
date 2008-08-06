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

#ifndef MAIN_DIALOG_H
#define MAIN_DIALOG_H

//#include "db_lib/db_field_label.h"
#include "db_lib/db_table_widget.h"
#include "db_lib/db_tab_widget.h"
#include "db_lib/db_search_widget.h"
#include "db_lib/db_form.h"
#include "db_lib/db_query_widget.h"
#include "db_lib/db_connection.h"
#include "db_lib/db_sql_combobox.h"

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QObject>
#include <QPushButton>
#include <QHBoxLayout>

#include <QItemSelectionModel>

class main_dialog : public QDialog
{
 Q_OBJECT
  public:
    main_dialog(const QString & name, QWidget *parent = 0);
    bool init(const db_connection *cnn, const QString& table_name);

  private slots:
    void open_search();
    void open_table();
    void open_form();
    void open_tab();
    void open_query();
    void open_systable();
    void display_text(const QString& txt);

  private:
    db_tab_widget *pv_tabw;
    db_table_widget *pv_tablew;
    db_form *pv_form;
    db_search_widget *pv_searchw;
    db_query_widget *pv_queryw;
    db_table_widget *pv_systable;
    db_sql_combobox *pv_sql_cbox;
    QString pv_table_name;
    QPushButton *pb_tabw;
    QPushButton *pb_tablew;
    QPushButton *pb_form;
    QPushButton *pb_searchw;
    QPushButton *pb_queryw;
    QPushButton *pb_systablew;
    QHBoxLayout *pv_hlayout;

    QItemSelectionModel *pv_selection_model;
};

#endif

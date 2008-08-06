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

#ifndef DB_QUERY_WIFGET_H
#define DB_QUERY_WIFGET_H

#include "db_connection.h"
#include "db_mem_lineedit.h"

#include <QWidget>
#include <QKeyEvent>
#include <QLineEdit>

class QSqlQuery;
class QTableWidget;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QTextEdit;
class QTextDocument;
class QPushButton;
class QPainter;
class QPrinter;
class QFile;
class QTextStream;

class db_query_widget : public QWidget
{
 Q_OBJECT
  public:
    db_query_widget(const QString &name, QWidget *parent = 0);
    /// Call this init function before any use
    bool init(const db_connection *cnn, const QString &SQL);
    bool new_query(const QString &SQL);

  private slots:
    bool submit_query();
    bool export_html();
    bool export_pdf();

  private:
    QSqlQuery *pv_query;    // Private instance of QSqlQuery
    QString pv_SQL;         // Contains the SQL query string
    const db_connection *pv_cnn;  // Connexion
    QVBoxLayout *pv_vlayout;
    QHBoxLayout *pv_hlayout;
    QLabel *lb_title, *lb_status;
    db_mem_lineedit *le_sql;
    QTextEdit *te_text;
    QTextDocument *pv_text_doc;
    QPushButton *pb_submit;
    QPushButton *pb_html;
    QPushButton *pb_pdf;
    QTableWidget *pv_table;
    //db_field_label pv_label;
    int pv_max_rows;
    bool generate_html();
    QString pv_html;
    bool write_file(QString path, QString txt);
    //QPainter *pv_painter;
    //QPrinter *pv_printer;
};

#endif

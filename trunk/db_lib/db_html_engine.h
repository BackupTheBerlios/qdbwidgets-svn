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

#ifndef DB_HTML_ENGINE_H
#define DB_HTML_ENGINE_H

#include "db_relational_model.h"

#include <QString>
#include <QStringList>
//#include <

class db_html_engine
{
 public:
  db_html_engine();
  bool set_model(const db_relational_model *model);
  void tests();

 private:
  QString get_header(int column);
  QString get_data(int row, int column);
  QString get_column_view();
  db_relational_model *pv_model;
  QStringList pv_headers;
};

#endif

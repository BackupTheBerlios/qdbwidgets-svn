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

#include "db_mem_lineedit.h"

#include <QKeyEvent>
#include <QRegExp>
#include <iostream>

/*
  db_sql_lineedit implementation
*/

db_mem_lineedit::db_mem_lineedit()
{
  pv_text_index = -1;
  connect(this, SIGNAL(editingFinished()), this, SLOT(add_text()));
}

void db_mem_lineedit::add_text()
{
  if(!pv_text_list.contains(displayText())){
    pv_text_list << displayText();
    pv_text_index++;
  }
}

void db_mem_lineedit::keyPressEvent(QKeyEvent *event)
{
  switch(event->key()){
    case Qt::Key_Up:
        if(pv_text_index > 0){
          pv_text_index--;
          setText(pv_text_list.at(pv_text_index));
        }
        break;
    case Qt::Key_Down:
        if(pv_text_index < (pv_text_list.count()-1)){
          pv_text_index++;
          setText(pv_text_list.at(pv_text_index));
        }
        break;
    default:
      QLineEdit::keyPressEvent(event);
  }
}

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

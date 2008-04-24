#ifndef DB_MEM_LINEEDIT_H
#define DB_MEM_LINEEDIT_H

#include <QLineEdit>

class db_mem_lineedit : public QLineEdit
{
 Q_OBJECT

 public:
  db_mem_lineedit();

 private slots:
  void add_text();

 private:
  void keyPressEvent(QKeyEvent *event);
  QStringList pv_text_list; // Contains all typed texts
  int pv_text_index;    // Index of last insert texts
};


#endif

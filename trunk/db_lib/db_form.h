#ifndef DB_FORM_H
#define DB_FORM_H

#include "db_table_widget.h"
#include "db_tab_widget.h"
#include "db_query_widget.h"

#include <QWidget>
#include <QString>
#include <QList>
#include <QVariant>
#include <QSqlDatabase>

class QModelIndex;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class db_form : public QWidget
{
 Q_OBJECT
  public:
    db_form(const QString &name, QWidget *parent = 0);
    bool init(const db_connection *cnn, const QString &parent_table_name, const QString &child_table_name);
    void select();

  private slots:

  private:
    db_tab_widget *pv_parent;
    //db_table_widget *pv_parent;
    db_table_widget *pv_child;
    //db_tab_widget *pv_child;
    db_table_widget *pv_s_child;
    QVBoxLayout *pv_vlayout;
    QHBoxLayout *pv_hlayout;
    //const db_connection *pv_cnn;        // Pointer to the connection instance
};

#endif

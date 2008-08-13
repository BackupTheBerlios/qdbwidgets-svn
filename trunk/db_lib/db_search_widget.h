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

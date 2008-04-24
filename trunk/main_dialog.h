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
};

#endif

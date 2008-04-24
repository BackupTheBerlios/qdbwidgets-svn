#ifndef DB_TAB_WIDGET_H
#define DB_TAB_WIDGET_H

#include "db_connection.h"
#include "db_field_label.h"

#include <QWidget>

//class QSqlTableModel;
class QSqlRelationalTableModel;
class QSqlDatabase;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QLineEdit;
class QLabel;
class QPushButton;
class QModelIndex;

class db_tab_entry : public QObject
{
 Q_OBJECT
  public:
    db_tab_entry();
    bool init(const QString &label_text);
    void set_editable(bool editable);
    void set_never_editable();
    void hide_field(bool hide);
    QGridLayout *get_layout();
    void set_data(const QString &data);
    QString get_data();
    bool data_changed();
  private slots:
    void slot_edited();
  private:
    QGridLayout *pv_layout;
    QLineEdit *pv_text;
    QLabel *pv_label;
    bool pv_data_change;
    bool pv_never_editable;
};

class db_tab_widget : public QWidget
{
 Q_OBJECT
  public:
    db_tab_widget(const QString &name, QWidget *parent = 0);
    /// Call this init function before any use
    bool init(const db_connection *cnn, const QString &table_name);
    /// Return number of fields (columns)
    int field_count();
    /// Activate navigation buttons
    bool activate_nav();
    QString get_field_name(int col);
    QString get_table_name();
    QSqlRecord get_record(int row);
    void select();
    QStringList get_header_data();
    void set_editable(bool editable);
    /**
      Relation management:
        1: Define fileds in the relation, in the parent instance, and in child instance (order is important!)
        2: connect the signal sig_current_data_changed from parent to slot slot_current_data_changed in child
    **/
    /// add a filed to relation - AS parent form
    void add_as_parent_relation_field(const QString &field_name);
    /// add a filed to relation - AS child form
    void add_as_child_relation_field(const QString &field_name);
    /// Hide a field
    void hide_field(const QString &field_name);

  signals:
    void sig_current_data_changed(const QStringList &relations_values);

  private slots:
    void goto_next();
    void goto_first();
    void goto_previous();
    void goto_last();
    bool goto_row(int row);
    void insert_record();
    bool update_record();
    void refresh_record();
    bool save_record();
    void delete_record();
    // AS child instance, this slot recieve needed data for relations...
    void slot_current_data_changed(const QStringList &relations_values);
    void as_child_before_insert(QSqlRecord &rec);

  private:
    QSqlRelationalTableModel *pv_table_model;
    //QSqlDatabase pv_cnn;              // Pointer to the database connection instance
    //const db_connection *pv_cnn;              // Pointer to the database connection instance
    db_field_label pv_label;          // Field labels
    QStringList pv_field_names;
    QHBoxLayout *pv_hlayout;
    QVBoxLayout *pv_vlayout;
    QGridLayout *pv_layout;
    db_tab_entry *pv_entry;
    // Navigation...
    QPushButton *pb_first;            // Goto first row
    QPushButton *pb_next;            // Goto next row
    QPushButton *pb_previous;
    QPushButton *pb_last;
    QPushButton *pb_insert;
    QPushButton *pb_save;
    QPushButton *pb_delete;
    int pv_current_row;               // Position
    bool pv_is_editable;
    bool is_empty(QSqlRecord &rec);
    bool pv_add_filter(const QString &field_name, const QString &val);
    QString pv_filter;
    QString pv_filter_user_args;
    // List of fields "implqué" in relation, AS parent form
    QStringList pv_as_parent_relation_fields;
    // List of fields "implqué" in relation, AS child form
    QStringList pv_as_child_relation_fields;
    QStringList pv_as_child_relation_values;
};

#endif

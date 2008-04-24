#ifndef DB_TABLE_WIDGET_H
#define DB_TABLE_WIDGET_H

#include "db_connection.h"
#include "db_field_label.h"

#include <QWidget>

class QSqlRelationalTableModel;
class QTableView;
class QSqlDatabase;
class QVBoxLayout;
class QHBoxLayout;
class QHeaderView;
class QSqlRecord;
class QModelIndex;
class QSqlIndex;
class QPushButton;

class db_table_widget : public QWidget
{
 Q_OBJECT
  public:
    db_table_widget(const QString &name, QWidget *parent = 0);
    /// Call this init function before any use
    bool init(const db_connection *cnn, const QString &table_name);
    /// Return number of fields (columns)
    int field_count();
    QString get_field_name(int col);
    QString get_table_name();
    QSqlRecord get_record(int row);
    void select();
    QString get_filter();
    QStringList get_header_data();
    void set_editable(bool editable);
    /// Get the list of Primary keys
    QList<QVariant> get_PKs();
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
    void insert_record();
    void delete_record();
    // Recieve from TableView, and emit to slot_current_data_changed
    void current_data_changed(const QModelIndex &index);
    // AS child, update relation list
    void as_child_before_insert(QSqlRecord &rec);
    // AS child instance, this slot recieve needed data for relations...
    void slot_current_data_changed(const QStringList &relations_values);

  private:
    QSqlRelationalTableModel *pv_table_model;
    QTableView *pv_table_view;
    //QSqlDatabase pv_cnn;            // Pointer to the database connection instance
    //db_connection *pv_cnn;            // Pointer to the database connection instance
    db_field_label pv_label;          // Field labels
    QStringList pv_field_names;
    QVBoxLayout *pv_layout;
    QHBoxLayout *pv_hlayout;
    QPushButton *pb_insert;
    QPushButton *pb_delete;
    bool add_filter(const QString &field_name, const QString &val);
    QString pv_filter;
    // List of fields "implqué" in relation, AS parent form
    QStringList pv_as_parent_relation_fields;
    // List of fields "implqué" in relation, AS child form
    QStringList pv_as_child_relation_fields;
    QStringList pv_as_child_relation_values;
};

#endif

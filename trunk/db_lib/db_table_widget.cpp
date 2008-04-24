#include "db_table_widget.h"
#include <iostream>
#include <QString>
#include <QStringList>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSqlIndex>
#include <QSqlField>
#include <QList>
#include <QVariant>
#include <QPushButton>
#include <QMessageBox>
#include <QPoint>

db_table_widget::db_table_widget(const QString &name, QWidget *parent)
    : QWidget(parent)
{
  pv_table_model = 0;
  pv_table_view = 0;
}

bool db_table_widget::init(const db_connection *cnn, const QString &table_name)
{
  pv_layout = new QVBoxLayout(this);
  pv_hlayout = new QHBoxLayout(this);

  // Init the labels
  pv_label.init(cnn, table_name);

  setWindowTitle(table_name);

  // Init the model
  pv_table_model = new QSqlRelationalTableModel(this, cnn->get_db());
  pv_table_model->setTable(table_name);

  // Set the header data - take from field_label class instance
  int i;
  QStringList lst;
  for(i=0; i<pv_table_model->columnCount() ;i++){
    lst.insert(i, pv_table_model->headerData(i, Qt::Horizontal).toString());
    pv_table_model->setHeaderData(i , Qt::Horizontal, pv_label.get_label(lst.at(i)));
  }
  // Store fileds names
  pv_field_names = lst;

  //pv_FKs = new QSqlIndex();

  // Init the view
  pv_table_view = new QTableView(this);
  pv_table_view->setModel(pv_table_model);
  pv_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
  pv_table_view->setSelectionBehavior(QAbstractItemView::SelectItems);
  pv_table_view->resizeColumnsToContents();
  pv_table_view->setSortingEnabled(true);
  pv_table_view->sortByColumn(0, Qt::AscendingOrder);

  pv_layout->addWidget(pv_table_view);

  pb_insert = new QPushButton(tr("Insert"));
  pb_delete = new QPushButton(tr("Delete"));
  pv_hlayout->addWidget(pb_insert);
  pv_hlayout->addWidget(pb_delete);

  pv_layout->addLayout(pv_hlayout);

  connect(pb_insert, SIGNAL(clicked()), this, SLOT(insert_record()));
  connect(pb_delete, SIGNAL(clicked()), this, SLOT(delete_record()));
  // Signal from tableView whenn seleted (to update child)
  connect( pv_table_view->selectionModel(),
    SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
    this, SLOT(current_data_changed(const QModelIndex&)) );
  connect(
    pv_table_model, SIGNAL(beforeInsert(QSqlRecord&)),
    this, SLOT(as_child_before_insert(QSqlRecord&) )
  );

  return true;
}

void db_table_widget::insert_record()
{
  int row = 0;
  pv_table_model->insertRows(row, 1);
}

void db_table_widget::delete_record()
{
  QModelIndex index = pv_table_view->currentIndex();
  if(index.isValid()){
    QMessageBox msgbox(QMessageBox::Warning, tr("Delete one row"), tr("Do you really want to delete selected item ?"), QMessageBox::Ok | QMessageBox::Cancel);
    int rep = msgbox.exec();
    if(rep == QMessageBox::Ok){
      int row = index.row();
      if(!pv_table_model->removeRow(row)){
        QMessageBox err_box(QMessageBox::Critical, tr("Delete failed"), tr("Delete failed"), QMessageBox::Ok);
      }
    }
  }
}

// Recieve from TableView, and emit to slot_current_data_changed - AS parent
void db_table_widget::current_data_changed(const QModelIndex &index)
{
  if(index.isValid()){
    // Data as parent
    QSqlRecord rec = pv_table_model->record(index.row());
    QStringList relations_values;
    int i=0;
    for(i=0; i<pv_as_parent_relation_fields.count(); i++){
      relations_values << rec.value(i).toString();
    }
    // Emit to child
    emit sig_current_data_changed(relations_values);
  }else{
    QStringList relations_values;
    int i=0;
    for(i=0; i<pv_as_parent_relation_fields.count(); i++){
      relations_values << "-1";
    }
    // Emit to child
    emit sig_current_data_changed(relations_values);
  }
}

void db_table_widget::as_child_before_insert(QSqlRecord &rec)
{
  int i=0;
  for(i=0; i<pv_as_child_relation_values.count(); i++){
    rec.setValue(pv_as_child_relation_fields.value(i), pv_as_child_relation_values.value(i));
  }
}

// AS child, recieve data_changed signal FROM parent
void db_table_widget::slot_current_data_changed(const QStringList &relations_values)
{
  pv_filter.clear();
  pv_as_child_relation_values = relations_values;
  // For each field, add criteria to the filter
  int i=0;
  for(i=0; i<pv_as_child_relation_values.count(); i++){
    if(!add_filter(pv_as_child_relation_fields.value(i), pv_as_child_relation_values.value(i))){
      std::cerr << "db_table_widget::" << __FUNCTION__ << ": relation criteria failed" << std::endl;
    }
  }
  select();
  // We can have another child. So we act as parent of this other child
  // So, we select the first row, then take the index. If valid, emit signal
  QModelIndex index = pv_table_view->indexAt(QPoint(0,0));
  emit current_data_changed(index);
}

void db_table_widget::select()
{
  if(!pv_table_model->select()){
    std::cerr << "db_table_widget::" << __FUNCTION__ << ": select() failed" << std::endl;
    std::cerr << pv_table_model->lastError().text().toStdString().c_str() << std::endl;
  }
}

void db_table_widget::set_editable(bool editable)
{
  if(editable == true){
    pv_table_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
  }else{
    pv_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
  }
}

int db_table_widget::field_count()
{
  return pv_table_model->columnCount();
}

QString db_table_widget::get_field_name(int col)
{
  return pv_field_names.at(col);
}

QString db_table_widget::get_table_name()
{
  return pv_table_model->tableName();
}

QSqlRecord db_table_widget::get_record(int row)
{
  return pv_table_model->record(row);
}

QStringList db_table_widget::get_header_data()
{
  QStringList headers;
  int i;
  for(i=0; i<pv_table_model->columnCount() ;i++){
    headers << pv_table_model->headerData(i , Qt::Horizontal).toString();
  }
  return headers;
}

QList<QVariant> db_table_widget::get_PKs()
{
  QModelIndex index;
  index = pv_table_view->currentIndex();
  int i;
  QList<QVariant> PKs_list;
  QVariant var;
  // get the list of primary keys
  for(i=0; i<pv_table_model->primaryKey().count(); i++){
    var = pv_table_model->primaryKey().field(i).name();
    PKs_list << var;
  }
  return PKs_list;
}

void db_table_widget::add_as_parent_relation_field(const QString &field_name)
{
  pv_as_parent_relation_fields << field_name;
}

void db_table_widget::add_as_child_relation_field(const QString &field_name)
{
  pv_as_child_relation_fields << field_name;
}

bool db_table_widget::add_filter(const QString &field_name, const QString &val)
{
  if(!pv_filter.isEmpty()){
    pv_filter = pv_filter + " AND ";
  }
  // Get the field parameters. For this, get the row 0 (If empty, fields info should be aviable)
  QSqlRecord rec = get_record(0);
  QSqlField field = rec.field(field_name);
  // Select the correct SQL operator according to data type.
  if((field.type() == QVariant::Int)||(field.type() == QVariant::Double)){
    pv_filter = pv_filter + field_name + "=" + val;
  }else if(field.type() == QVariant::String){
    pv_filter = pv_filter + field_name + "='" + val + "' ";
  }else{
    std::cerr << "db_table_widget::" << __FUNCTION__ << ": unknow data type. Filter not added" << std::endl;
    return false;
  }
  //std::cout << "db_table_widget::" << __FUNCTION__ << "**-> Filter: " << pv_filter.toStdString().c_str() << std::endl;
  pv_table_model->setFilter(pv_filter);
  //pv_table_model->select();
  return true;
}

QString db_table_widget::get_filter()
{
  return pv_table_model->filter();
}

void db_table_widget::hide_field(const QString &field_name)
{
  int index = pv_table_model->fieldIndex(field_name);
  pv_table_view->setColumnHidden(index, true);
}

#include "db_tab_widget.h"
#include <iostream>
#include <QString>
#include <QSqlRelationalTableModel>
#include <QSqlError>
#include <QSqlField>
#include <QVariant>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSqlRecord>
#include <QPushButton>
#include <QSqlIndex>
#include <QModelIndex>
#include <QMessageBox>

/* Tab entry impl */

db_tab_entry::db_tab_entry()
{
  pv_layout = 0;
  pv_text = 0;
}

bool db_tab_entry::init(const QString &label_text)
{
  pv_layout = new QGridLayout;
  pv_label = new QLabel;
  pv_label->setText(label_text);
  pv_layout->addWidget(pv_label);
  pv_text = new QLineEdit;
  pv_layout->addWidget(pv_text);
  pv_data_change = false;
  connect(pv_text, SIGNAL(editingFinished()),this,SLOT(slot_edited()));
  pv_never_editable = false;
  return true;
}

void db_tab_entry::set_editable(bool editable)
{
  if((editable == true)&&(pv_never_editable == false)){
    pv_text->setReadOnly(false);
  }else{
    pv_text->setReadOnly(true);
  }
}

void db_tab_entry::set_never_editable()
{
  pv_never_editable = true;
  set_editable(false);
}

void db_tab_entry::hide_field(bool hide)
{
  if(hide){
    pv_text->setVisible(false);
    pv_label->setVisible(false);
  }else{
    pv_text->setVisible(true);
    pv_label->setVisible(true);
  }
}

QGridLayout* db_tab_entry::get_layout()
{
  return pv_layout;
}

void db_tab_entry::set_data(const QString &data)
{
  pv_text->setText(data);
}

QString db_tab_entry::get_data()
{
  return pv_text->text();
}

void db_tab_entry::slot_edited()
{
  std::cout << "Data changed\n";
  pv_data_change = true;
}

bool db_tab_entry::data_changed()
{
  if(pv_data_change == true){
    pv_data_change = false;
    return true;
  }
  return false;
}

// NOTE: Do I have to destruct the Widgets contained in th db_tab_entry class ?? (no parent given...)

/* Tab widget impl */

db_tab_widget::db_tab_widget(const QString &name, QWidget *parent)
    : QWidget(parent)
{
  pv_table_model = 0;
}

bool db_tab_widget::init(const db_connection *cnn, const QString &table_name)
{
  // Init the labels
  pv_label.init(cnn, table_name);

  setWindowTitle(table_name);

  // Init the model
  //pv_table_model = new QSqlTableModel(this, cnn.get_db());
  pv_table_model = new QSqlRelationalTableModel(this, cnn->get_db());
  pv_table_model->setTable(table_name);
  pv_table_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  // Set the header data - take from field_label class instance
  int i;
  QStringList lst;
  for(i=0; i<pv_table_model->columnCount() ;i++){
    lst.insert(i, pv_table_model->headerData(i, Qt::Horizontal).toString());
    pv_table_model->setHeaderData(i , Qt::Horizontal, pv_label.get_label(lst.at(i)));
  }
  // Store fileds names
  pv_field_names = lst;

  // Vertical layout
  pv_vlayout = new QVBoxLayout(this);
  // Horizontal layout (for nav)
  pv_hlayout = new QHBoxLayout(this);
  // Init layout and arranges labels and texts
  pv_layout = new QGridLayout(this);
  pv_entry = new db_tab_entry[pv_table_model->columnCount()];
  // Get fields infos
  QSqlRecord record = pv_table_model->record(0);
  for(i=0; i<pv_table_model->columnCount() ;i++){
    pv_entry[i].init(pv_table_model->headerData(i, Qt::Horizontal).toString());
    pv_layout->addLayout(pv_entry[i].get_layout(),i,0);
    // Setup field specifics FIXME don't work..
/*    if(record.field(i).isAutoValue()){
      pv_entry[i].set_never_editable();
      std::cout << "Autovalue\n";
    }
*/  }
  // Add the grid layout to the Vlayout
  pv_vlayout->addLayout(pv_layout);
  // Add the hlayout (nav) below...
  pv_vlayout->addLayout(pv_hlayout);
  activate_nav();

  connect(
    pv_table_model, SIGNAL(beforeInsert(QSqlRecord&)),
    this, SLOT(as_child_before_insert(QSqlRecord&) )
  );

  // Go to the first row
  goto_first();

  return true;
}

void db_tab_widget::select()
{
  pv_table_model->setSort(0, Qt::AscendingOrder);
  if(!pv_table_model->select()){
    std::cerr << "db_table_widget::" << __FUNCTION__ << ": select() failed" << std::endl;
    std::cerr << pv_table_model->lastError().text().toStdString().c_str() << std::endl;
  }
  goto_first();
}

int db_tab_widget::field_count()
{
  return pv_table_model->columnCount();
}

bool db_tab_widget::activate_nav()
{
  pb_first = new QPushButton(tr("First"));
  pv_hlayout->addWidget(pb_first);
  pb_previous = new QPushButton(tr("Previous"));
  pv_hlayout->addWidget(pb_previous);
  pb_next = new QPushButton(tr("Next"));
  pv_hlayout->addWidget(pb_next);
  pb_last = new QPushButton(tr("Last"));
  pv_hlayout->addWidget(pb_last);
  pb_insert = new QPushButton(tr("Insert"));
  pv_hlayout->addWidget(pb_insert);
  pb_save = new QPushButton(tr("Save"));
  pv_hlayout->addWidget(pb_save);
  pb_delete = new QPushButton(tr("Delete"));
  pv_hlayout->addWidget(pb_delete);

  // Connect slots
  connect(pb_next, SIGNAL(clicked()), this, SLOT(goto_next()));
  connect(pb_first, SIGNAL(clicked()), this, SLOT(goto_first()));
  connect(pb_previous, SIGNAL(clicked()), this, SLOT(goto_previous()));
  connect(pb_last, SIGNAL(clicked()), this, SLOT(goto_last()));
  connect(pb_insert, SIGNAL(clicked()), this, SLOT(insert_record()));
  connect(pb_save, SIGNAL(clicked()), this, SLOT(save_record()));
  connect(pb_delete, SIGNAL(clicked()), this, SLOT(delete_record()));

  return true;
}

void db_tab_widget::goto_next()
{
  update_record();
  if(pv_current_row < pv_table_model->rowCount()-1){
    pv_current_row++;
    goto_row(pv_current_row);
  }
}

void db_tab_widget::goto_previous()
{
  update_record();
  if(pv_current_row > 0){
    pv_current_row--;
    goto_row(pv_current_row);
  }
}

void db_tab_widget::goto_first()
{
  update_record();
  goto_row(0);
}

void db_tab_widget::goto_last()
{
  update_record();
  while(pv_table_model->canFetchMore()){
    pv_table_model->fetchMore();
  }
  goto_row(pv_table_model->rowCount() -1);
}

bool db_tab_widget::goto_row(int row)
{
  int i;
  QSqlRecord record;
  QStringList relations_values;
  record = pv_table_model->record(row);
  if(is_empty(record)){
    for(i=0; i<record.count(); i++){
      pv_entry[i].set_data("");
      pv_entry[i].set_editable(false);
      std::cout << "Empty record \n";
    }
  }else{
    // Set all data in the entry fields
    for(i=0; i<record.count(); i++){
      pv_entry[i].set_editable(true);
      pv_entry[i].set_data(record.value(i).toString());
    }
  }
  pv_current_row = row;
  // relations updates - AS parent
  for(i=0; i<pv_as_parent_relation_fields.count(); i++){
    relations_values << record.value(i).toString();
  }
  emit sig_current_data_changed(relations_values);
  return true;
}

// AS child, recieve data_changed signal FROM parent
void db_tab_widget::slot_current_data_changed(const QStringList &relations_values)
{
  pv_filter.clear();
  pv_as_child_relation_values = relations_values;
  // For each field, add criteria to the filter
  int i=0;
  for(i=0; i<pv_as_child_relation_values.count(); i++){
    if(!pv_add_filter(pv_as_child_relation_fields.value(i), pv_as_child_relation_values.value(i))){
      std::cerr << "db_tab_widget::" << __FUNCTION__ << ": relation criteria failed" << std::endl;
    }
  }
  select();
}

void db_tab_widget::as_child_before_insert(QSqlRecord &rec)
{
  int i=0;
  for(i=0; i<pv_as_child_relation_values.count(); i++){
    rec.setValue(pv_as_child_relation_fields.value(i), pv_as_child_relation_values.value(i));
  }
}

bool db_tab_widget::update_record()
{
  int i, diff=0, text_changed=0;
  // Check if data where chaged by user
  for(i=0; i<pv_table_model->columnCount(); i++){
    if(pv_entry[i].data_changed()){
      text_changed++;
    }
  }
  if(text_changed>0){
    QSqlRecord record;
    record = pv_table_model->record(pv_current_row);
    for(i=0; i<record.count(); i++){
      if(record.value(i).toString() != pv_entry[i].get_data()){
        diff++;
      }
    }
  }
  if(diff > 0){
    QMessageBox msgbox(QMessageBox::Warning, tr("Update one row"), tr("Data modified: Commit changes ?"), QMessageBox::Ok | QMessageBox::No);
    int rep = msgbox.exec();
    if(rep == QMessageBox::Ok){
      // Update any record in model
      save_record();
    }

    if(pv_current_row>=0){
      goto_row(pv_current_row);
    }else{
      goto_last();
    }
  }
  return true;
}

void db_tab_widget::refresh_record()
{
  int i;
  //std::cout << "Current row: " << pv_current_row << std::endl;
  QSqlRecord record;
  record = pv_table_model->record(pv_current_row);
  // Update any record in model
  for(i=0; i<record.count(); i++){
    pv_entry[i].set_data(record.value(i).toString());
  }
}

bool db_tab_widget::save_record()
{
  int i;
  QSqlRecord record;
  record = pv_table_model->record(pv_current_row);
  // Update any record item
  for(i=0; i<record.count(); i++){
    record.setValue(i, pv_entry[i].get_data());
  }
  // If new record (insert_record() set the pv_current_row to -1)
  if(pv_current_row == -1){
    if(!pv_table_model->insertRecord(-1, record)){
      std::cerr << "db_tab_widget::" << __FUNCTION__ << ": Record insertion failed\n";
      std::cerr << "-> Return text: " << pv_table_model->lastError().text().toStdString().c_str() << std::endl;
      return false;
    }
    pv_current_row = pv_table_model->rowCount()-1;
  }else{
    if(!pv_table_model->setRecord(pv_current_row, record)){
      std::cerr << "db_tab_widget::" << __FUNCTION__ << ": Record update failed\n";
      std::cerr << "-> Return text: " << pv_table_model->lastError().text().toStdString().c_str() << std::endl;
      return false;
    }
  }
  // Submit
  std::cerr << "db_tab_widget::" << __FUNCTION__ << ": -> Saving record\n";
  if(!pv_table_model->submitAll()){
      std::cerr << "db_tab_widget::" << __FUNCTION__ << ": Record submit failed\n";
      std::cerr << "-> Return text: " << pv_table_model->lastError().text().toStdString().c_str() << std::endl;
  }
  //refresh_record();
  goto_row(pv_current_row);
  //emit signal_data_update();
  return true;
}

QString db_tab_widget::get_field_name(int col)
{
  return pv_field_names.at(col);
}

QString db_tab_widget::get_table_name()
{
  return pv_table_model->tableName();
}

QSqlRecord db_tab_widget::get_record(int row)
{
  return pv_table_model->record(row);
}

QStringList db_tab_widget::get_header_data()
{
  QStringList headers;
  int i;
  for(i=0; i<pv_table_model->columnCount() ;i++){
    headers << pv_table_model->headerData(i , Qt::Horizontal).toString();
  }
  return headers;
}

void db_tab_widget::set_editable(bool editable)
{
  pv_is_editable = editable;
}

bool db_tab_widget::is_empty(QSqlRecord &rec)
{
  int i=0;
  QString data;
  for(i=0; i<rec.count(); i++){
    data = rec.value(i).toString();
    if(!data.isEmpty()){
      return false;
    }
  }
  return true;
}

void db_tab_widget::add_as_parent_relation_field(const QString &field_name)
{
  pv_as_parent_relation_fields << field_name;
}

void db_tab_widget::add_as_child_relation_field(const QString &field_name)
{
  pv_as_child_relation_fields << field_name;
}

void db_tab_widget::hide_field(const QString &field_name)
{
  int index = pv_table_model->fieldIndex(field_name);
  if(index>0){
    pv_entry[index].hide_field(true);
  }
}

bool db_tab_widget::pv_add_filter(const QString &field_name, const QString &val)
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
    std::cerr << "db_tab_widget::" << __FUNCTION__ << ": unknow data type. Filter not added" << std::endl;
    return false;
  }
  //std::cout << "db_table_widget::" << __FUNCTION__ << "**-> Filter: " << pv_filter.toStdString().c_str() << std::endl;
  pv_table_model->setFilter(pv_filter);
  //pv_table_model->select();
  return true;
}

void db_tab_widget::insert_record()
{
  int i;
  QSqlRecord record;
  QSqlField field;
  record = pv_table_model->record(pv_current_row);
  for(i=0; i<record.count(); i++){
    field = record.field(i);
    pv_entry[i].set_editable(true);
    pv_entry[i].set_data(field.defaultValue().toString());
  }
  pv_current_row = -1;
}

void db_tab_widget::delete_record()
{
  QMessageBox msgbox(QMessageBox::Warning, tr("Delete one row"), tr("Do you really want to delete selected item ?"), QMessageBox::Ok | QMessageBox::Cancel);
  int rep = msgbox.exec();
  if(rep == QMessageBox::Ok){
    //QModelIndex index = pv_table_view->currentIndex();
    int row = pv_current_row;
    if(!pv_table_model->removeRow(row)){
      QMessageBox err_box(QMessageBox::Critical, tr("Delete failed"), tr("Delete failed"), QMessageBox::Ok);
    }
    std::cerr << "db_tab_widget::" << __FUNCTION__ << ": -> Deleting record\n";
    if(!pv_table_model->submitAll()){
        std::cerr << "db_tab_widget::" << __FUNCTION__ << ": Record submit failed\n";
        std::cerr << "-> Return text: " << pv_table_model->lastError().text().toStdString().c_str() << std::endl;
    }
    refresh_record();
  }
}

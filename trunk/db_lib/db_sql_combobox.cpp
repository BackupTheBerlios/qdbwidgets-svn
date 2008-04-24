#include "db_sql_combobox.h"
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QComboBox>
#include <QTableView>
#include <QString>
#include <QSize>
#include <iostream>

/*
  Delegate implementation
*/

db_sqlcb_item_delegate::db_sqlcb_item_delegate()
{
  pv_max_width = 120;
  pv_widths = 0;
  pv_height = 0;
}

db_sqlcb_item_delegate::~db_sqlcb_item_delegate()
{
  if(pv_widths != 0){
    delete[] pv_widths;
    pv_widths = 0;
  }
}

/* Called from view - return the size stored for concerned column */
QSize db_sqlcb_item_delegate::sizeHint (const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
  QSize size(0, pv_height);
  if(pv_widths != 0){
    size.setWidth(pv_widths[index.column()]);
  }
  return size;
}

void db_sqlcb_item_delegate::set_height(int height)
{
  pv_height = height;
}
/* We set a size for each column regarding on field size, but <= pv_max_width */
void db_sqlcb_item_delegate::set_widths(QSqlRecord record)
{
  int i = 0;
  pv_widths = new int[record.count()];
  for(i=0; i<record.count(); i++){
    pv_widths[i] = record.field(i).length();
    if(pv_widths[i] > pv_max_width){
      pv_widths[i] = pv_max_width;
    }
  }
}

/*
  sql combobox view implementation
*/

int db_sqlcb_view::sizeHintForRow (int row) const
{
  return 10;
}

/* 
  sql combobox implementation
*/

db_sql_combobox::db_sql_combobox(const QString &name, QWidget *parent)
  : QComboBox(parent)
{
  pv_data_column = 0;
  pv_display_column = 0;
  pv_show_data_column = true;
}

bool db_sql_combobox::init(const db_connection *cnn)
{
  pv_cnn = cnn;
  pv_query = new QSqlQueryModel;

  pv_view = new db_sqlcb_view;
  pv_item_delegate = new db_sqlcb_item_delegate;

  pv_view->setSelectionMode(QAbstractItemView::SingleSelection);

  setModel(pv_query);
  setEditable(true);
  setView(pv_view);
  // To adjust size of the displayed list, I find nothing else..Must be AFTER setView()
  setItemDelegate(pv_item_delegate);

  connect(this, SIGNAL(activated(int)), this, SLOT(slot_data(int)));

  return true;
}


bool db_sql_combobox::new_query(const QString &SQL)
{
  int fields_count = 0;
  pv_query->setQuery(SQL, pv_cnn->get_db());
  if(pv_query->lastError().isValid()) {
    std::cerr << "db_sql_combobox::" << __FUNCTION__ << ": query execution failed" << std::endl;
    std::cerr << "-> " << pv_query->lastError().text().toStdString().c_str() << std::endl;
    setEditText(tr("#Error"));
    return false;
  }
  fields_count = pv_query->record(0).count();
  setMinimumContentsLength(12*fields_count); // 10 character per field
  pv_item_delegate->set_height(40);
  pv_item_delegate->set_widths(pv_query->record(0));
  pv_view->resizeColumnsToContents();
  pv_view->resizeRowsToContents();
  return true;
}

bool db_sql_combobox::set_data_field(const QString &field_name)
{
  pv_data_column = pv_query->record(0).indexOf(field_name);
  if(pv_data_column < 0){
    std::cerr << "db_sql_combobox::" << __FUNCTION__ << ": field named \"" << field_name.toStdString().c_str() << "\" not found" << std::endl;
    return false;
  }
  setModelColumn(pv_display_column);
  return true;
}

void db_sql_combobox::show_data_field(bool show)
{
  int i = 0;
  pv_show_data_column = show;
  if(!pv_show_data_column){
    // Find the first showed column
    for(i=0; i<pv_query->record(0).count(); i++){
      if(!pv_view->isColumnHidden(i)){
        pv_display_column = i-1;
      }
    }
    pv_view->setColumnHidden(pv_data_column, true);
  }else{
   pv_view->setColumnHidden(pv_data_column, false);
  }
  setModelColumn(pv_display_column);
}

void db_sql_combobox::slot_data(int index)
{
  QString data;
  std::cout << "INDEX: " << index << std::endl;
  data = pv_query->record(index).value(pv_data_column).toString();
  // We set the selected item to the user if the data column is hidded
  if(!pv_show_data_column){
    setEditText(pv_query->record(index).value(pv_display_column).toString());
  }
  emit sig_data(data);
}

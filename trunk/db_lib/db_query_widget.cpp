#include "db_query_widget.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <iostream>
#include <QSqlError>
#include <QStringList>
#include <QSqlField>
#include <QVariant>

#include <QTextEdit>

#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QTextDocument>

#include <QFile>
#include <QFileDialog>
#include <QTextStream>


/*
  db_query_widget implementation
*/

db_query_widget::db_query_widget(const QString &name, QWidget *parent)
  : QWidget(parent)
{
  /// Define maximum of rows here
  pv_max_rows = 5000;
  if(!name.isEmpty()){
    setWindowTitle(name);
  }
}

bool db_query_widget::init(const db_connection *cnn, const QString &SQL)
{
  pv_cnn = cnn;
  pv_SQL = SQL;

  pv_vlayout = new QVBoxLayout(this);
  lb_title = new QLabel(tr("SQL query widget - Connection: " ) + pv_cnn->get_cnn_name());
  pv_vlayout->addWidget(lb_title);
  pv_table = new QTableWidget(this);
  pv_vlayout->addWidget(pv_table);
  le_sql = new db_mem_lineedit;
  pv_vlayout->addWidget(le_sql);
  pv_hlayout = new QHBoxLayout(this);
  pv_vlayout->addLayout(pv_hlayout);
  pb_submit = new QPushButton(tr("&Submit"));
  pv_hlayout->addWidget(pb_submit);
  pb_html = new QPushButton(tr("&Export HTML"));
  pv_hlayout->addWidget(pb_html);
  pb_pdf = new QPushButton(tr("Export PDF"));
  pv_hlayout->addWidget(pb_pdf);
  lb_status = new QLabel(tr("Ready"));
  pv_vlayout->addWidget(lb_status);

  //pv_printer = new QPrinter;
  //pv_painter = new QPainter;

  pv_text_doc = new QTextDocument(this);
  te_text = new QTextEdit;
  //te_text->show();

  pv_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

  pv_query = new QSqlQuery(pv_cnn->get_db());
  if(!pv_SQL.isEmpty()){
    new_query(pv_SQL);
  }

  //pv_label.init(cnn);

  connect(pb_submit, SIGNAL(clicked()),this, SLOT(submit_query()));
  connect(pb_html, SIGNAL(clicked()),this, SLOT(export_html()));
  connect(pb_pdf, SIGNAL(clicked()),this, SLOT(export_pdf()));
  return true;
}

bool db_query_widget::export_html()
{
  lb_status->setText(tr("Html export in progress..."));
  if(!generate_html()){
    std::cerr << "db_query_widget::" << __FUNCTION__ << ": Error while generating html" << std::endl;
    lb_status->setText(tr("Error while generating html"));
    return false;
  }
  pv_text_doc->setHtml(pv_html);
  te_text->setDocument(pv_text_doc);
  lb_status->setText(tr("Ready"));
  return true;
}

bool db_query_widget::export_pdf()
{
  generate_html();


  QPrinter printer;
  QPrintDialog printer_dialog(&printer, NULL);
  printer_dialog.exec();
  printer.setFullPage(true);
  QTextDocument facture;
  facture.setPlainText("Test");
  facture.print(&printer);

  //QPrinter printer;
/*  pv_printer->setOrientation(QPrinter::Portrait);
  pv_printer->setPageSize(QPrinter::A4);
  pv_printer->setResolution(72);
  pv_printer->setFullPage(false);
  pv_printer->setNumCopies(1);
  pv_printer->setOutputFileName("test.pdf");
  pv_printer->setOutputFormat(QPrinter::PdfFormat);
*/

  //QPrintDialog printer_dialog(&printer, this);
    //pv_text_doc->setHtml("hello");
    //pv_text_doc->setHtml("<table width=\"100%\" border=\"1\" cellspacing=0>\nhello\n</table>\n");
    //te_text->setDocument(pv_text_doc);
  //if(printer_dialog.exec()){
    //QPainter painter(&printer);

  //pv_painter->begin(pv_printer);

    pv_text_doc->setHtml(pv_html);
    //pv_text_doc->print(pv_printer);

    //pv_painter->end();
  //}
  return false; // nothing's working here...
}

bool db_query_widget::new_query(const QString &SQL)
{
  lb_status->setText(tr("Sending SQL query..."));
  pv_SQL = SQL;
  le_sql->setText(pv_SQL);
  if(!pv_query->exec(pv_SQL)){
    std::cerr << "db_query_widget::" << __FUNCTION__ << ": query execution failed" << std::endl;
    std::cerr << "-> " << pv_query->lastError().text().toStdString().c_str() << std::endl;
    lb_status->setText(tr("Query execution failed: ") + pv_query->lastError().text());
    return false;
  }
  // Count num rows and columns
  QSqlRecord record;
  int row=0, col=0, i;
  while(pv_query->next()){
    if(row >= pv_max_rows){
      break;
    }
    row++;
  }
  pv_query->first();
  record = pv_query->record();
  col = record.count();

  // Init the table
  pv_table->setColumnCount(col);
  for(i=0; i<col; i++){
    QTableWidgetItem *item = new QTableWidgetItem(record.field(i).name());
    pv_table->setHorizontalHeaderItem(i, item);
  }

  // Enter data
  pv_table->setSortingEnabled(false);
  pv_table->setRowCount(row);
  row = 0;
  while(pv_query->next()){
    if(row >= pv_max_rows){
      std::cerr << "db_query_widget::" << __FUNCTION__ << ": Maximum rows -> Aborting here" << std::endl;
      break;
    }
    record = pv_query->record();
    // Set each column
    for(i=0; i<col; i++){
      QTableWidgetItem *item = new QTableWidgetItem(record.value(i).toString());
      pv_table->setItem(row, i, item);
    }
    row++;
  }
  pv_table->setSortingEnabled(true);
  pv_table->resizeRowsToContents();
  pv_table->resizeColumnsToContents();
  lb_status->setText(tr("Ready"));
  return true;
}

bool db_query_widget::submit_query()
{
  return new_query(le_sql->text());
}


bool db_query_widget::generate_html()
{
  QString html;
  // Head..
  html = "<HTML>";
  html += "<TABLE BORDER=\"1\">";

  QTableWidgetItem *item;

  // Data
  int row, col;
  for(row=0; row<pv_table->rowCount(); row++){
  //for(row=0; row<10; row++){
    html += " <TR>\n";
    for(col=0; col<pv_table->columnCount(); col++){
    //for(col=0; col<10; col++){
      html += "   <TD>";
      item = pv_table->item(row, col);
      if(item != 0){
        html += item->text();
      }
      html += "   </TD>\n";
    }
    html += " </TR><BR>\n";
  }

  // End..
  html += "</TABLE>";
  html += "</HTML>";
  pv_html = html;
  //std::cerr << pv_html.toStdString().c_str() << std::endl;
  write_file("test.html", html);
  return true;
}

bool db_query_widget::write_file(QString path, QString txt)
{
  QFile file(path);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
    std::cerr << "db_query_widget::" << __FUNCTION__ << ": failed to write to " << path.toStdString().c_str() << std::endl;
    return false;
  }
  QTextStream txt_stream(&file);
  txt_stream << txt;
  return true;
}

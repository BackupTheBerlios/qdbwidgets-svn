#include "db_html_engine.h"
#include <iostream>

#include <QTextDocument>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>

db_html_engine::db_html_engine()
{
  pv_child_model = 0;
  pv_model = 0;
  pv_txt_edit = new QTextEdit;
  pv_doc = new QTextDocument;
}

bool db_html_engine::set_model(const db_relational_model *model)
{
  int i=0;
  pv_model = model;
  for(i=0; i<pv_model->columnCount(); i++){
    pv_headers.insert(i, pv_model->headerData(i, Qt::Horizontal).toString());
  }
  if(pv_model == 0){
    return false;
  }
  pv_child_model = pv_model->get_child_model();
  if(pv_child_model != 0){
    for(i=0; i<pv_child_model->columnCount(); i++){
      pv_child_headers.insert(i, pv_child_model->headerData(i, Qt::Horizontal).toString());
    }
  }
 // tests();

  return true;
}

void db_html_engine::tests()
{
  int i=0;
  QString html;

  html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n";
  html += "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
  html += "<style type=\"text/css\">\nh1 {color:blue; }\n</style>\n";
  html += "<h1>Titre</h1>\n";
  html += "<html>\n";

  html += get_parent_child_view(-1, false);

  html += "</html>\n";

  write_file("test.html", html);
 // std::cout << html.toStdString() << std::endl;
  pv_doc->setHtml(html);
  pv_txt_edit->setDocument(pv_doc);
  pv_txt_edit->show();
}

QString db_html_engine::get_header(int column, bool in_child)
{
  QString html, txt;
  html = " <td>";
  if(in_child){
    if(pv_child_model == 0){
      return html;  // No child, no child data
    }
    html += Qt::escape(pv_child_headers.at(column));
  }else{
    html += Qt::escape(pv_headers.at(column));
  }
  html += "</td>";
  return html;
}

QString db_html_engine::get_data(int row, int column, bool in_child)
{
  QString html;
  html = " <td>";
  if(in_child){
    if(pv_child_model == 0){
      return html;  // No child, no child data
    }
    html += Qt::escape(pv_child_model->get_text_data(row, column));
  }else{
    html += Qt::escape(pv_model->get_text_data(row, column));
  }
  html += "</td>";
  return html;
}

QString db_html_engine::get_column_view(int row, bool with_headers, bool in_child)
{
  int i=0, y=0, headers_count = 0, rows_count = 0;
  QString html;

  if(in_child){
    if(pv_child_model == 0){
      return html;  // No child, no child data
    }
    headers_count = pv_child_headers.count();
    rows_count = pv_child_model->rowCount();
  }else{
    headers_count = pv_headers.count();
    rows_count = pv_model->rowCount();
  }

  html = "<table border=1>\n";
  // write headers
  if(with_headers){
    html += "<tr>\n";
    for(i=0; i<headers_count; i++){
      html += get_header(i, in_child);
    }
    html += "\n</tr>\n";
  }
  // Write data
  if(row <= -1){
    for(i=0; i<rows_count; i++){
      html += "<tr>\n";
      for(y=0; y<headers_count; y++){
        html += get_data(i, y, in_child);
      }
      html += "\n</tr>\n";
    }
  }else{
    for(y=0; y<headers_count; y++){
      html += get_data(row, y, in_child);
    }
  }

  html += "</table>\n";
  return html;
}

QString db_html_engine::get_tab_view(int row, bool with_headers, bool in_child)
{
  int i=0, y=0, headers_count = 0, rows_count = 0;
  QString html;

  if(in_child){
    if(pv_child_model == 0){
      return html;  // No child, no child data
    }
    headers_count = pv_child_headers.count();
    rows_count = pv_child_model->rowCount();
  }else{
    headers_count = pv_headers.count();
    rows_count = pv_model->rowCount();
  }

  //html = "<table border=\"0\" width=\"50%\" bordercolor=\"#000000\" cellspacing=\"0\">\n";
  html = "<table border=1>\n";
  // Write data
  if(row <= -1){
    for(i=0; i<rows_count; i++){
      for(y=0; y<headers_count; y++){
        html += "<tr>\n";
        if(with_headers){
          html += get_header(y, in_child);
        }
        html += get_data(i, y, in_child);
        html += "\n</tr>\n";
      }
    }
  }else{
    for(y=0; y<headers_count; y++){
      html += "<tr>\n";
      if(with_headers){
        html += get_header(y, in_child);
      }
      html += get_data(row, y, in_child);
      html += "\n</tr>\n";
    }
  }
  html += "</table>\n";
  return html;
}

QString db_html_engine::get_parent_child_view(int row = -1, bool with_headers)
{
  int i=0, y=0;
  QString html;

  //html = "<table border=1>\n";
  // Write data
  if(row <= -1){
    for(i=0; i<pv_model->rowCount(); i++){
      // Parent data
      html += get_tab_view(i, with_headers, false);
      // child data
      html += get_column_view(-1, true, true);
    }
  }else{
    // Parent data
    html += get_tab_view(row, with_headers, false);
    // child data
    html += get_column_view(-1, true, true);
  }
  //html += "</table>\n";
  return html;
}

bool db_html_engine::write_file(QString path, QString txt)
{
  QFile file(path);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
    std::cerr << "db_html_engine::" << __FUNCTION__ << ": failed to write to " << path.toStdString().c_str() << std::endl;
    return false;
  }
  QTextStream txt_stream(&file);
  txt_stream << txt;
  return true;
}

#include "db_html_engine.h"
#include <iostream>

#include <QTextDocument>

db_html_engine::db_html_engine()
{
}

bool db_html_engine::set_model(const db_relational_model *model)
{
  int i=0;
  pv_model = model;
  for(i=0; i<pv_model->columnCount(); i++){
    pv_headers.insert(i, pv_model->headerData(i, Qt::Horizontal).toString());
  }

  tests();

  return true;
}

void db_html_engine::tests()
{
  int i=0;
  QString html;

  html = get_column_view();
  std::cout << html.toStdString() << std::endl;
}

QString db_html_engine::get_header(int column)
{
  QString html, txt;
  html = " <td>";
  html += Qt::escape(pv_headers.at(column));
  html += "</td>";
  return html;
}

QString db_html_engine::get_data(int row, int column)
{
  QString html;
  html = " <td>";
  html += Qt::escape(pv_model->get_text_data(row, column));
  html += "</td>";
  return html;
}

QString db_html_engine::get_column_view()
{
  int i=0, y=0;
  QString html;

  html = "<table border=1>\n";
  // write headers
  html += "<tr>\n";
  for(i=0; i<pv_headers.count(); i++){
    html += get_header(i);
  }
  html += "\n</tr>\n";
  // Write data
  for(i=0; i<pv_model->rowCount(); i++){
    html += "<tr>\n";
    for(y=0; y<pv_headers.count(); y++){
      html += get_data(i, y);
    }
    html += "\n</tr>\n";
  }

  html += "</table>";
  return html;
}

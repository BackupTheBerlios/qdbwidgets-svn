#include "db_html_engine.h"
#include <iostream>
#include "db_relation.h"

#include <QTextDocument>
#include <QTextFrame>
#include <QTextBrowser>
//#include <QTextStream>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QModelIndex>
#include <QUrl>
#include <QColor>
#include <QSqlRecord>

#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>

db_html_engine::db_html_engine()
{
  //pv_child_model = 0;
  pv_table_model = 0;
  pv_cnn = 0;
  pv_html_view = new db_html_view;
  pv_txt_edit = new QTextEdit;
  pv_txt_edit = new QTextEdit;
  pv_doc = new QTextDocument;
}

/// NOTE: delete childs ?

bool db_html_engine::init(const db_connection *cnn, const QString &table_name)
{
  pv_cnn = cnn;

  pv_table_model = new db_relational_model(pv_cnn, table_name);
  /// NOTE: Ne devrait pas être transmis comme ca, mais passé en paramètre.. (?)
  pv_html_view->set_model(pv_table_model);
  pv_html_view->set_pks_hidden();

  /// relations (adresses)
  db_relation relation(pv_table_model->tableName());
  relation.add_parent_relation_field("soc_cod_pk");
  relation.set_child_table("adresse_societe_tbl");
  relation.add_child_relation_field("adr_soc_cod_fk");

  /// relations (contacts)
  db_relation con_relation(pv_table_model->tableName());
  con_relation.add_parent_relation_field("soc_cod_pk");
  con_relation.set_child_table("contact_societe_tbl");
  con_relation.add_child_relation_field("con_soc_cod_fk");

  /// relations (détails contacts)
  db_relation det_con_relation("contact_societe_tbl");
  det_con_relation.add_parent_relation_field("con_soc_id_pk");
  det_con_relation.set_child_table("detail_contact_societe_tbl");
  det_con_relation.add_child_relation_field("det_con_soc_id_fk");

  /// Attributes
  db_html_view_attributes attributes, con_attributes, det_con_attributes;
  attributes.set_left_margin(10);
  attributes.set_top_margin(10);
  con_attributes.set_left_margin(10);
  con_attributes.set_top_margin(10);
  det_con_attributes.set_left_margin(20);
  det_con_attributes.set_top_margin(10);

  /// Add childs, and hide relation fields
  add_child_table(relation, attributes, relation.get_child_relation_fields(), true);
  add_child_table(con_relation, con_attributes, con_relation.get_child_relation_fields(), true);
  add_child_table(det_con_relation, det_con_attributes, det_con_relation.get_child_relation_fields(), true);

  return true;
}

QString db_html_engine::get_html_header()
{
  QString html;

  html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n";
  html += "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
  html += "<style type=\"text/css\">\nh1 {color:blue; }\n</style>\n";
  //html += "<link rel=\"stylesheet\" type=\"text/css\" href=\"format.css\">";
  //html += "<h1>Titre</h1>\n";

  //html += "<p class=\"child\">Titre en rouge</p>\n";

  html += "<html>\n";
  html += "<head>\n";
  html += "<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">\n";
  html += "</head>\n";

  return html;
}

QString db_html_engine::get_html_line(const QString text, bool br)
{
  QString html;
  html = Qt::escape(text);
  if(br){
    html += "<br>\n";
  }
  return html;
}

void db_html_engine::set_doc_header(const QString &html)
{
  pv_html_doc_header = "<table width=\"100%\">\n <tr>\n";
  pv_html_doc_header += "  <td align=\"center\">" + html + "\n";
  pv_html_doc_header += "  </td>\n </tr>\n</table>\n";
}

void db_html_engine::set_doc_top(const QString &html_left, const QString &html_right)
{
  // Global top table
  pv_html_doc_top = "<table border=\"0\" width=\"100%\" cellspacing=\"1\" cellpadding=\"3\">\n";
  pv_html_doc_top += "<tr>\n<td width=\"70%\">\n";
  // Top left 
  //pv_html_doc_top += "<td width=\"70%\">";
  pv_html_doc_top += "<table border=\"0\" cellspacing=\"1\" cellpadding=\"3\" width=\"100%\">\n<tr><td>\n";
  pv_html_doc_top += html_left;
  // end top left 
  pv_html_doc_top += "</td></tr></table>\n";
  pv_html_doc_top += "</td><td>\n";
  // Top right 
  pv_html_doc_top += "<table border=\"1\" cellspacing=\"0\" cellpadding=\"3\" width=\"100%\">\n<tr><td>\n";
  pv_html_doc_top += html_right;
  // end top right 
  pv_html_doc_top += "</td></tr></table>\n";
  // End global top table
  pv_html_doc_top += "</td></tr>\n</table>\n";
  //pv_html_doc_top = "";
}

QString db_html_engine::get_top_right_data(const QString &table_name, const QString &filter)
{
  int i=0, y=0;
  QString html;
  db_relational_model table_model(pv_cnn, table_name);
  table_model.setFilter(filter);
  table_model.select();

  for(i=0; i < table_model.rowCount(); i++){
    for(y=0; y < table_model.columnCount(); y++){
      html += get_html_line(table_model.get_text_data(i, y), true);
    }
  }

  return html;
}

void db_html_engine::tests()
{
  int i=0;
  QString html, css, tmp1, tmp2;
  QColor bg_color("blue");
  QColor tables_bg_color("white");
  QColor tables_border_color("blue");

  css  = "table {";
  css  += "  border-style: solid;";
  css  += "  border-color: " + tables_border_color.name() + ";";
  css  += "  background-color: " + tables_bg_color.name() + ";";
  css  += "}";

  if(!pv_table_model->select()){
    std::cerr << "db_html_engine::" << __FUNCTION__ << ": select() failed" << std::endl;
  }

  html = get_html_header();

  //set_doc_header(get_html_line("Entete"));
  set_doc_header("<h1>Test report </h1><img src=\"logo_left.png\"></img>");


  tmp1 = get_html_line("Donnes de gauche", true);
  tmp1 += get_html_line("Donnes de gauche, 2eme ligne", false);

  //html += get_top_right_data("", "");
  html += get_data();
  html += "</html>\n";

  //std::cout << html.toStdString() << std::endl;
  //pv_doc->addResource(QTextDocument::StyleSheetResource, QUrl("format.css"), css);
  pv_doc->setDefaultStyleSheet(css);
  pv_doc->setHtml(html);

  //std::cout << pv_doc->toHtml().toStdString() << std::endl;
  //write_file("test.html", pv_doc->toHtml());
  write_file("test.html", html);
//  pv_txt_edit->setDocument(pv_doc);
//  pv_txt_edit->show();

  pv_txt_edit->setDocument(pv_doc);
  pv_txt_edit->show();

  QPrinter printer;
  QPrintDialog *dialog = new QPrintDialog(&printer);

//  QTextFrame *root_frame = pv_doc->rootFrame();

  dialog->setWindowTitle("Print Document");
  // Setup printer
  if(dialog->exec() != QDialog::Accepted){
    return;
  }

  // Initial document size
  //std::cout << "---> pageCount(): " << pv_doc->pageCount() << std::endl;
  std::cout << "---> Initial size().width(): " << pv_doc->size().width() << std::endl;
  std::cout << "---> Initial size().height(): " << pv_doc->size().height() << std::endl;

  // Print parameters
  std::cout << "---> Printer: page height: " << printer.pageRect().height() << std::endl;
  std::cout << "---> Printer: page width: " << printer.pageRect().width() << std::endl;
  // Resize document's width:
  qreal area, new_height;
  // Actual document area:
  area = pv_doc->size().width() * pv_doc->size().height();
  std::cout << "---> area: " << area << std::endl;
  // Calculate new height, based on area and printer's given width
  new_height = area / (qreal)printer.pageRect().width();
  // Setup new dimetions
  QSizeF doc_size((qreal)printer.pageRect().width(), new_height);
  pv_doc->setPageSize(doc_size);

  std::cout << "---> pageCount(): " << pv_doc->pageCount() << std::endl;
  std::cout << "---> size().width(): " << pv_doc->size().width() << std::endl;
  std::cout << "---> size().height(): " << pv_doc->size().height() << std::endl;

  //std::cout << "---> blockCount(): " << pv_doc->blockCount() << std::endl;


/*
  if (pv_txt_edit->textCursor().hasSelection())
        dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    if (dialog->exec() != QDialog::Accepted)
        return;

    pv_txt_edit->print(&printer);
*/
}

QString db_html_engine::get_data()
{
  int i = 0, y = 0, field_index = 0;
  QString html, data;

  for(i=0; i<pv_table_model->rowCount(); i++){
    QModelIndex index = pv_table_model->create_index(i, 0);
    //html += pv_html_view->get_column_row(i);
    html += pv_html_doc_header;
    field_index = pv_table_model->record(i).indexOf("soc_nom");
    data = pv_table_model->get_text_data(i, field_index);
    std::cout << "-------------> soc_nom: " << data.toStdString() << std::endl;
    set_doc_top("Test gauche", get_top_right_data("societe_vue", "soc_nom='" + data + "'"));
    html += pv_html_doc_top;
    html += "<table border=\"0\" style=\"margin-top: 15px \" cellspacing=\"0\" cellpadding=\"3\">\n";
    html += pv_html_view->get_tab_row(i, true);
    html += "</table>\n";
    for(y=0; y < pv_child_views.size(); y++){
      //html += "<table border=\"0\" style=\"margin-top: 15px \" cellspacing=\"0\" cellpadding=\"3\">\n";
      //html += pv_child_views.at(y)->get_user_table_name();
      html += pv_child_views.at(y)->get_column_view(true);
      //html += "</table>";
    }
    pv_table_model->current_row_changed(index);
  }

  return html;
}

bool db_html_engine::add_child_table(const db_relation &relation, db_html_view_attributes attributes,
                                      const QStringList hidden_fields, 
                                      bool pks_hidden)
{
  int i = 0;
  //bool sub_child = false;

  db_relational_model *parent_model = 0;
  // The parent could be in the childs list, 
  // the new child will then become a sub-child.
  // Fisrt, search in the childs list if one have the name
  // given as parent table in the relation. If non, the parent is pv_parent.
  for(i = 0; i < pv_child_models.size(); i++){
    if(pv_child_models.at(i)->tableName() == relation.get_parent_table()){
      // parent will be this child
      parent_model = pv_child_models.at(i);
    }
  }
  if(parent_model == 0){
    // Not found in childs, parent will be pv_parent
    parent_model = pv_table_model;
    //sub_child = false;
  }else{
    //sub_child = true;
  }
  // Create the new child model
  pv_child_models.append(new db_relational_model(pv_cnn, relation.get_child_table()));
  pv_child_models.last()->setEditStrategy(QSqlTableModel::OnRowChange);
  parent_model->add_child_model(pv_child_models.last());
  parent_model->set_relation(relation);

  // html views
  pv_child_views.append(new db_html_view);
  pv_child_views.last()->set_model(pv_child_models.last());
  pv_child_views.last()->set_fields_hidden(hidden_fields);
  if(pks_hidden){
    pv_child_views.last()->set_pks_hidden();
  }
  pv_child_views.last()->set_attributes(attributes);
  return true;
}


/*
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
*/

/*
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
*/

/*
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
*/

/*
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
*/

/*
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
*/

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

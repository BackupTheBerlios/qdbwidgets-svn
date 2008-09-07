#include "db_html_engine.h"
#include <iostream>
#include "db_relation.h"

#include <QTextDocument>
#include <QTextBrowser>
//#include <QTextStream>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QModelIndex>
#include <QUrl>
#include <QColor>

db_html_engine::db_html_engine()
{
  //pv_child_model = 0;
  pv_table_model = 0;
  pv_cnn = 0;
  pv_html_view = new db_html_view;
  pv_txt_edit = new QTextEdit;
  pv_txt_browser = new QTextBrowser;
  pv_doc = new QTextDocument;
}
/*
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
*/


bool db_html_engine::init(const db_connection *cnn, const QString &table_name)
{
  pv_cnn = cnn;

  pv_table_model = new db_relational_model(pv_cnn, table_name);
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

void db_html_engine::tests()
{
  int i=0;
  QString html, css;
  QColor bg_color("blue");
  QColor tables_bg_color("white");
  QColor tables_border_color("blue");
/*
  //css  = ".child { color:#DE0000; ";
  css  = ".child { color:#DE0000; ";
  css  += "  font-family:sans-serif;";
  css  += "  margin-left: 50px;";
  css  += "  margin-right: 50px;";
  css  += "  background-color: " + bg_color.name() + ";";
  css  += "}";
*/
  css  = "table {";
  css  += "  border-style: solid;";
  css  += "  border-color: " + tables_border_color.name() + ";";
  css  += "  background-color: " + tables_bg_color.name() + ";";
  css  += "}";

  if(!pv_table_model->select()){
    std::cerr << "db_html_engine::" << __FUNCTION__ << ": select() failed" << std::endl;
  }

  html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n";
  html += "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
  html += "<style type=\"text/css\">\nh1 {color:blue; }\n</style>\n";
  //html += "<link rel=\"stylesheet\" type=\"text/css\" href=\"format.css\">";
  html += "<h1>Titre</h1>\n";

  html += "<p class=\"child\">Titre en rouge</p>\n";

  html += "<html>\n";
  html += "<head>\n";
  html += "<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">\n";
  html += "</head>\n";

  //html += "<table border=1>\n";

  html += get_data();

  //html += "</table>\n";

  html += "</html>\n";


  //std::cout << html.toStdString() << std::endl;
  //pv_doc->addResource(QTextDocument::StyleSheetResource, QUrl("format.css"), css);
  pv_doc->setDefaultStyleSheet(css);
  pv_doc->setHtml(html);

  //std::cout << pv_doc->toHtml().toStdString() << std::endl;
  write_file("test.html", pv_doc->toHtml());
//  pv_txt_edit->setDocument(pv_doc);
//  pv_txt_edit->show();

  pv_txt_browser->setDocument(pv_doc);
  pv_txt_browser->show();
}

QString db_html_engine::get_data()
{
  int i = 0, y = 0;
  QString html;

  for(i=0; i<pv_table_model->rowCount(); i++){
    QModelIndex index = pv_table_model->create_index(i, 0);
    //html += pv_html_view->get_column_row(i);
    html += "<table border=\"0\" style=\"margin-top: 15px \" cellspacing=\"0\" cellpadding=\"3\">\n";
    html += pv_html_view->get_tab_row(i, true);
    html += "</table>\n";
    for(y=0; y < pv_child_views.size(); y++){
      //html += "</table>\n";
      //html += "<span class=\"child\">\n";
      html += pv_child_views.at(y)->get_user_table_name();
      //html += "</span>\n";
      //html += "<p class=\"child\">\n";
      html += pv_child_views.at(y)->get_column_view(true);
      //html += "</p>\n";
      //html += "<table border=1>\n";
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

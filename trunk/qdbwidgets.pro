######################################################################
# Automatically generated by qmake (2.01a) lun. juil. 30 19:39:55 2007
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += main_dialog.h \
           db_lib/db_connection.h db_lib/db_connection_dlg.h db_lib/db_field_label.h db_lib/db_table_widget.h db_lib/db_tab_widget.h \
           db_lib/db_relational_model.h db_lib/db_tab_view.h db_lib/db_relation.h \
           db_lib/db_search_widget.h db_lib/db_form.h db_lib/db_query_widget.h db_lib/db_sql_combobox.h db_lib/db_mem_lineedit.h


SOURCES += main.cpp main_dialog.cpp \
           db_lib/db_connection.cpp db_lib/db_connection_dlg.cpp db_lib/db_field_label.cpp db_lib/db_table_widget.cpp db_lib/db_tab_widget.cpp \
           db_lib/db_relational_model.cpp db_lib/db_tab_view.cpp db_lib/db_relation.cpp \
           db_lib/db_search_widget.cpp db_lib/db_form.cpp db_lib/db_query_widget.cpp db_lib/db_sql_combobox.cpp db_lib/db_mem_lineedit.cpp

# Libs
QT += sql
#ifndef PAGE1_H
#define PAGE1_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include "database.h"

class Page1 : public QWidget {
    Q_OBJECT
public:
    Page1(QStackedWidget *stack, Database &db);  // Добавьте параметр Database &db
private:
    Database &db;  // Ссылка на объект базы данных
    QStackedWidget *stack;
};

#endif // PAGE1_H

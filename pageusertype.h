#ifndef PAGEUSERTYPE_H
#define PAGEUSERTYPE_H

#include <QWidget>
#include <QStackedWidget>
#include "database.h"

class PageUserType : public QWidget
{
    Q_OBJECT

public:
    PageUserType(QStackedWidget *stack, Database &db);

private:
    QStackedWidget *stack;
    Database &db;
};

#endif // PAGEUSERTYPE_H

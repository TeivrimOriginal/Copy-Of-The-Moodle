#ifndef PAGE4_H
#define PAGE4_H

#include <QWidget>
#include <QStackedWidget>
#include "database.h"

class Page4 : public QWidget
{
    Q_OBJECT

public:
    Page4(QStackedWidget *stack, Database &db);
    ~Page4();

private:
    QStackedWidget *stack;
    Database &db;

    void handleLogin();
};

#endif // PAGELOGIN_H

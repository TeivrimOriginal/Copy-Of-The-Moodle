// page3.h

#ifndef PAGE3_H
#define PAGE3_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include "database.h"

class Page3 : public QWidget {
    Q_OBJECT

public:
    Page3(QStackedWidget *stack, Database &db);  // Объявление конструктора с параметрами

private:
    QStackedWidget *stack;
    Database &db;
    QLabel *infoLabel;
};

#endif // PAGE3_H

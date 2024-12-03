#ifndef PAGE2_H
#define PAGE2_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include "database.h"
#include <QStackedWidget>
class Page2 : public QWidget {
    Q_OBJECT
public:
    Page2(QStackedWidget *stack, Database &db);
private:
    QStackedWidget *stack;
    Database &db;
    void handleRegistration();
    void handleLogin();
};

#endif // PAGE2_H

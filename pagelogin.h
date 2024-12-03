#ifndef PAGLOGIN_H
#define PAGLOGIN_H

#include <QWidget>
#include <QStackedWidget>
#include <QLineEdit> // Для использования QLineEdit

class Database;

class PageLogin : public QWidget
{
    Q_OBJECT

public:
    PageLogin(QStackedWidget *stack, Database &db);
    ~PageLogin();

private:
    QStackedWidget *stack;
    Database &db;

    QLineEdit *loginEdit;    // Переменная для логина
    QLineEdit *passwordEdit; // Переменная для пароля

    void handleLogin();
};

#endif // PAGLOGIN_H

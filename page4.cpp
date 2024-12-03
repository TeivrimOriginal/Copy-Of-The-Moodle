#include "page4.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

Page4::Page4(QStackedWidget *stack, Database &db) : stack(stack), db(db)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *label = new QLabel("Вход в систему", this);
    layout->addWidget(label);

    QLineEdit *loginEdit = new QLineEdit(this);
    loginEdit->setPlaceholderText("Логин");

    QLineEdit *passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("Пароль");

    QPushButton *actionButton = new QPushButton("Войти", this);
    layout->addWidget(loginEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(actionButton);

    connect(actionButton, &QPushButton::clicked, [this, loginEdit, passwordEdit]() {
        handleLogin();
    });

    this->setLayout(layout);
}

Page4::~Page4() {}

void Page4::handleLogin()
{
    QString login = loginEdit->text();
    QString password = passwordEdit->text();

    // Проверяем, что логин и пароль не пустые
    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Логин и пароль обязательны для заполнения");
        return;
    }

    // Проверка логина и пароля в базе данных
    int userId = db.checkLoginCredentials(login, password);
    if (userId != -1) {
        // Сохраняем ID пользователя в файл
        QFile file("cash.txt");
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream out(&file);
            out << userId;
            file.close();
        }

        // Переход на страницу 3 (или любую нужную)
        stack->setCurrentIndex(2); // Индекс страницы после успешного входа
    } else {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль");
    }
}

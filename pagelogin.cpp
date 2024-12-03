#include "pageLogin.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include "database.h"
#include <QDebug>
#include <QCoreApplication>
#include <QProcess>
#include <QApplication>
PageLogin::PageLogin(QStackedWidget *stack, Database &db) : stack(stack), db(db)
{
    QVBoxLayout *layout = new QVBoxLayout(this);


    QPushButton *registerButton = new QPushButton("Вернуться назад", this);

    layout->addWidget(registerButton);

    connect(registerButton, &QPushButton::clicked, [stack]() {
        stack->setCurrentIndex(0);  // Переход на страницу регистрации
    });

    QLabel *label = new QLabel("Вход в систему", this);
    layout->addWidget(label);

    // Инициализируем loginEdit и passwordEdit как члены класса
    loginEdit = new QLineEdit(this);
    loginEdit->setPlaceholderText("Логин");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);  // Для скрытия пароля
    passwordEdit->setPlaceholderText("Пароль");

    QPushButton *actionButton = new QPushButton("Войти", this);

    layout->addWidget(loginEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(actionButton);

    connect(actionButton, &QPushButton::clicked, this, &PageLogin::handleLogin);

    this->setLayout(layout);
}

PageLogin::~PageLogin() {}

void PageLogin::handleLogin()
{
    qDebug() << "Вход произведен";
    QString login = loginEdit->text();    // Используем переменные-члены класса
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

        // Получаем путь к исполняемому файлу приложения
        QString program = QApplication::applicationFilePath();

        // Завершаем текущее приложение
        QProcess::startDetached(program, QStringList());

        // Закрываем текущее приложение
        qApp->quit();

    } else {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль");
    }
}

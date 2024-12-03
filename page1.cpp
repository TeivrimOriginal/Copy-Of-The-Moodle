#include "page1.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QCoreApplication>
#include <QProcess>
#include <QApplication>
#include "database.h"
#include <QTextStream>
Page1::Page1(QStackedWidget *stack, Database &db) : stack(stack), db(db) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QFile file("cash.txt");

    // Проверка на существование файла
    if (file.exists()) {
        // Чтение ID пользователя из файла cash.txt
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            int userId = in.readLine().toInt();  // Чтение ID из файла
            file.close();

            // Получаем данные пользователя из базы данных
            QVariant userData = db.getUserData(userId);
            if (userData.isValid()) {
                QVariantMap userMap = userData.toMap();
                QString role = userMap["role"].toString(); // Извлекаем роль пользователя

                // Выводим текст с типом пользователя
                QLabel *roleLabel = new QLabel(QString("Ваш тип пользователя: %1").arg(role), this);
                layout->addWidget(roleLabel);
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось получить данные о пользователе.");
            }
        }
        QPushButton *registerButton = new QPushButton("Профиль", this);
        layout->addWidget(registerButton);

        connect(registerButton, &QPushButton::clicked, [stack]() {
            stack->setCurrentIndex(2);  // Переход на страницу 1
        });

        QPushButton *Course = new QPushButton("Главная страница", this);
        layout->addWidget(Course);

        connect(Course, &QPushButton::clicked, [stack]() {
            stack->setCurrentIndex(4);  // Переход на страницу 1
        });
        QPushButton *lesson = new QPushButton("Главная страница", this);
        layout->addWidget(lesson);

        connect(lesson, &QPushButton::clicked, [stack]() {
            stack->setCurrentIndex(7);  // Переход на страницу 1
        });
        QPushButton *exitButton = new QPushButton("Выход", this);
        layout->addWidget(exitButton);

        connect(exitButton, &QPushButton::clicked, [this]() {
            QFile file("cash.txt");
            if (file.exists() && file.remove()) {
                QMessageBox::information(this, "Успех", "Файл cash.txt удален.");
                // Перезапускаем приложение
                QString program = QApplication::applicationFilePath();
                QProcess::startDetached(program, QStringList());
                qApp->quit();
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось удалить файл cash.txt.");
            }
        });
    } else {
        // Если файл "cash.txt" не существует, показываем кнопки регистрации и входа
        QPushButton *registerButton = new QPushButton("Регистрация", this);
        QPushButton *loginButton = new QPushButton("Вход", this);

        layout->addWidget(registerButton);
        layout->addWidget(loginButton);

        connect(registerButton, &QPushButton::clicked, [stack]() {
            stack->setCurrentIndex(1);  // Переход на страницу регистрации
        });

        connect(loginButton, &QPushButton::clicked, [stack]() {
            stack->setCurrentIndex(3);  // Переход на страницу входа
        });
    }

    // Если файла cash.txt нет, показываем кнопку "Выход"
    if (!file.exists()) {

    }

    // Устанавливаем layout для страницы
    this->setLayout(layout);
}

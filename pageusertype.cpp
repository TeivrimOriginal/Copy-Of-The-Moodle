#include "pageUserType.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QLabel>
#include "database.h"
#include <QApplication>
#include <QDebug>
PageUserType::PageUserType(QStackedWidget *stack, Database &db) : stack(stack), db(db) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    qDebug() << "тИП ПОЛЬЗЛОВАТЕЛЯ";
    QFile file("cash.txt");
        stack->setWindowTitle("Курсы");
    // Проверяем существует ли файл "cash.txt"
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            int userId = in.readLine().toInt();  // Получаем ID пользователя
            file.close();

            // Получаем данные о пользователе из базы данных
            QVariant userData = db.getUserData(userId);
            if (userData.isValid()) {
                QVariantMap userMap = userData.toMap();
                QString role = userMap["role"].toString(); // Извлекаем роль пользователя

                // В зависимости от роли выводим соответствующий текст
                QLabel *roleLabel = new QLabel(this);
                QLabel *roleLabel1 = new QLabel(this);
                if (role == "Обучающийся") {
                    QPushButton *back = new QPushButton("Назад", this);
                    layout->addWidget(back);

                    connect(back, &QPushButton::clicked, [stack]() {
                        stack->setCurrentIndex(1);  // Переход на страницу 1
                    });
                    roleLabel->setText("Ваш тип пользователя: Обучающийся");
                    QPushButton *Course = new QPushButton("Мои Курсы", this);
                    layout->addWidget(Course);

                    connect(Course, &QPushButton::clicked, [stack]() {
                        stack->setCurrentIndex(5);  // Переход на страницу 1
                    });

                } else if (role == "Наставник") {
                    QPushButton *back = new QPushButton("Назад", this);
                    layout->addWidget(back);

                    connect(back, &QPushButton::clicked, [stack]() {
                        stack->setCurrentIndex(1);  // Переход на страницу 1
                    });
                    roleLabel->setText("Ваш тип пользователя: Наставник");
                    QPushButton *Course = new QPushButton("Курсы", this);
                    layout->addWidget(Course);

                    connect(Course, &QPushButton::clicked, [stack]() {
                        stack->setCurrentIndex(5);  // Переход на страницу 1
                    });

                } else if (role == "Управляющий") {
                    QPushButton *back = new QPushButton("Назад", this);
                    layout->addWidget(back);

                    connect(back, &QPushButton::clicked, [stack]() {
                        stack->setCurrentIndex(5);  // Переход на страницу 1
                    });
                    roleLabel->setText("Ваш тип пользователя: Управляющий");
                    QPushButton *Course = new QPushButton("Курсы", this);
                    layout->addWidget(Course);

                    connect(Course, &QPushButton::clicked, [stack]() {
                        stack->setCurrentIndex(5);  // Переход на страницу 1
                    });
                } else if (role == "Администратор") {
                    QPushButton *back = new QPushButton("Назад", this);
                    layout->addWidget(back);

                    connect(back, &QPushButton::clicked, [stack]() {
                        stack->setCurrentIndex(5);  // Переход на страницу 1
                    });
                    roleLabel->setText("Ваш тип пользователя: Администратор");
                    QPushButton *Course = new QPushButton("Мои Курсы", this);
                    layout->addWidget(Course);

                    connect(Course, &QPushButton::clicked, [stack]() {
                        stack->setCurrentIndex(5);  // Переход на страницу 1
                    });
                } else {
                    roleLabel->setText("Неизвестный тип пользователя");
                }
                roleLabel1->setText(role);

                layout->addWidget(roleLabel1);  // Добавляем метку с ролью
                layout->addWidget(roleLabel);  // Добавляем метку с ролью
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось загрузить данные о пользователе.");
            }
        }
    } else {
        // Если файл "cash.txt" не существует, показываем кнопку для выхода
        QPushButton *exitButton = new QPushButton("Выход", this);
        layout->addWidget(exitButton);

        connect(exitButton, &QPushButton::clicked, [this]() {
            QFile file("cash.txt");
            if (file.exists() && file.remove()) {
                QMessageBox::information(this, "Успех", "Файл cash.txt удален.");
                qApp->quit();  // Закрываем приложение
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось удалить файл cash.txt.");
            }
        });
    }

    // Устанавливаем layout для страницы
    this->setLayout(layout);
}

// page3.cpp

#include "page3.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
Page3::Page3(QStackedWidget *stack, Database &db) : stack(stack), db(db) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    infoLabel = new QLabel("Информация о пользователе", this);
    layout->addWidget(infoLabel);

    QPushButton *registerButton = new QPushButton("Вернуться назад", this);
    layout->addWidget(registerButton);

    connect(registerButton, &QPushButton::clicked, [stack]() {
        stack->setCurrentIndex(0);  // Переход на страницу 1
    });

    QFile file("cash.txt");
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString userIdString = in.readLine(); // Чтение ID пользователя как строки
        bool ok;
        int userId = userIdString.toInt(&ok); // Преобразуем в int

        file.close();

        if (ok) {
            QVariant userData = db.getUserData(userId);
            if (userData.isValid()) {
                QVariantMap userMap = userData.toMap(); // Приводим к QVariantMap
                QString userInfo = QString("ID: %1\nЛогин: %2\nИмя: %3\nФамилия: %4\nУчебное заведение: %5\nГод рождения: %6\nПол: %7\nРоль: %8")
                                       .arg(userMap["id"].toString())
                                       .arg(userMap["login"].toString())
                                       .arg(userMap["name"].toString())
                                       .arg(userMap["surname"].toString())
                                       .arg(userMap["university"].toString())
                                       .arg(userMap["birth_date"].toString())
                                       .arg(userMap["gender"].toString())
                                       .arg(userMap["role"].toString());

                infoLabel->setText(userInfo); // Устанавливаем текст на лейбл
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось загрузить информацию о пользователе.");
            }
        } else {
            QMessageBox::warning(this, "Ошибка", "Неверный ID пользователя в файле.");
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Не найден файл с данными пользователя.");
    }

    this->setLayout(layout);
}

#include "page2.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QStackedWidget>
#include "page1.h"
#include "page2.h"
#include "page3.h"
#include "database.h"
#include <QCoreApplication>
#include <QProcess>
Page2::Page2(QStackedWidget *stack, Database &db) : stack(stack), db(db) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QPushButton *registerButton = new QPushButton("Вернуться назад", this);

    layout->addWidget(registerButton);

    connect(registerButton, &QPushButton::clicked, [stack]() {
        stack->setCurrentIndex(0);  // Переход на страницу регистрации
    });
    QLabel *label = new QLabel("Регистрация или Вход", this);
    layout->addWidget(label);

    QLineEdit *loginEdit = new QLineEdit(this);
    loginEdit->setPlaceholderText("Логин");

    QLineEdit *passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("Пароль");

    QLineEdit *nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("Имя");

    QLineEdit *surnameEdit = new QLineEdit(this);
    surnameEdit->setPlaceholderText("Фамилия");

    QLineEdit *universityEdit = new QLineEdit(this);
    universityEdit->setPlaceholderText("Учебное заведение");

    QLineEdit *birthDateEdit = new QLineEdit(this);
    birthDateEdit->setPlaceholderText("Год рождения");

    QComboBox *genderComboBox = new QComboBox(this);
    genderComboBox->addItems({"Мужской", "Женский"});

    QComboBox *roleComboBox = new QComboBox(this);
    roleComboBox->addItems({"Обучающийся", "Наставник", "Управляющий", "Администратор"});

    QPushButton *actionButton = new QPushButton("Зарегистрироваться", this);
    layout->addWidget(loginEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(nameEdit);
    layout->addWidget(surnameEdit);
    layout->addWidget(universityEdit);
    layout->addWidget(birthDateEdit);
    layout->addWidget(genderComboBox);
    layout->addWidget(roleComboBox);
    layout->addWidget(actionButton);

    connect(actionButton, &QPushButton::clicked, [this, loginEdit, passwordEdit, nameEdit, surnameEdit, universityEdit, birthDateEdit, genderComboBox, roleComboBox, stack, &db]() {
        QString login = loginEdit->text();
        QString password = passwordEdit->text();
        QString name = nameEdit->text();
        QString surname = surnameEdit->text();
        QString university = universityEdit->text();
        QString birthDate = birthDateEdit->text();
        QString gender = genderComboBox->currentText();
        QString role = roleComboBox->currentText();

        // Проверяем, что логин и пароль не пустые
        if (login.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Логин и пароль обязательны для заполнения");
            return;
        }

        // Проверяем, существует ли уже такой логин
        if (db.checkUserExist(login)) {
            QMessageBox::warning(this, "Ошибка", "Пользователь с таким логином уже существует");
            return;
        }

        // Добавляем нового пользователя
        int userId = db.addUser(login, password, name, surname, university, birthDate, gender, role);
        if (!(userId < 0)) {
            // Создаем и записываем уникальный ID в файл cash.txt
            QFile file("cash.txt");
            if (file.open(QIODevice::WriteOnly)) {
                QTextStream out (&file);
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
            // Сообщение об ошибке
            QMessageBox::warning(this, "Ошибка", "Ошибка при регистрации");
        }
    });
}

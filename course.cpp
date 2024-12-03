#include "course.h"
#include <QScrollArea>
#include <QFile>
#include <QTextStream>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFileDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QDebug>
#include <QSqlError>
#include <QDateTime>  // Для работы с датой и временем
#include "TemporaryData.h"

Course::Course(QStackedWidget *stack, Database &db) : QWidget(), stack(stack), db(db) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    int userId = getUserIdFromCash();

    if (userId == -1) {
        QMessageBox::critical(this, "Ошибка", "Не удалось найти ID пользователя.");
        return;
    }

    QPushButton *back = new QPushButton("Назад", this);
    layout->addWidget(back);

    connect(back, &QPushButton::clicked, [stack]() {
        stack->setCurrentIndex(4);  // Переход на страницу 5
    });

    // Создание окна прокрутки
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);  // Устанавливаем, чтобы содержимое изменяло размер в зависимости от содержимого
    QWidget *contentWidget = new QWidget();  // Контейнер для содержимого
    scrollArea->setWidget(contentWidget);    // Устанавливаем контейнер в качестве виджета

    QVBoxLayout *scrollLayout = new QVBoxLayout(contentWidget);

    loadUserPage(userId, scrollLayout);  // Fixed the argument type here

    // Устанавливаем высоту окна прокрутки 300 пикселей
    scrollArea->setFixedHeight(300);

    // Добавляем в основной layout окно прокрутки
    layout->addWidget(scrollArea);

    qDebug() << "КУРСЫ";
    stack->setWindowTitle("Курсы");
}

Course::~Course() {
    close();
}

bool Course::open() {
    if (!db.open()) {
        QMessageBox::critical(this, "Ошибка подключения", "Не удалось подключиться к базе данных.");
        return false;
    }
    return true;
}

void Course::close() {
    db.close();
}

int Course::getUserIdFromCash() {
    QFile file("cash.txt");
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString userIdString = in.readLine(); // Чтение ID пользователя как строки
        bool ok;
        int userId = userIdString.toInt(&ok); // Преобразуем в int
        file.close();

        if (ok) {
            return userId;
        }
    }
    return -1; // Возвращаем -1, если не удалось считать ID
}

QString Course::getUserRole(int userId) {
    QVariant userData = db.getUserData(userId);
    if (userData.isValid()) {
        QVariantMap userMap = userData.toMap();
        return userMap["role"].toString();
    }
    return "Неизвестная роль";  // Возвращаем строку по умолчанию
}

void Course::loadUserPage(int userId, QVBoxLayout *layout) {
    QString role = getUserRole(userId);

    QLabel *roleLabel = new QLabel("Роль: " + role, this);
    layout->addWidget(roleLabel);

    if (role == "Обучающийся") {
        loadStudentPage(layout, userId);
    } else if (role == "Наставник") {
        loadMentorPage(layout, userId);
    } else if (role == "Управляющий") {
        loadManagerPage(layout, userId);
    } else if (role == "Администратор") {
        loadAdminPage(layout);
    } else {
        QLabel *errorLabel = new QLabel("Неизвестная роль!", this);
        layout->addWidget(errorLabel);
    }
}

void Course::loadStudentPage(QVBoxLayout *layout, int userId) {
    QSqlQuery query;

    // Получаем ID курсов, связанных с пользователем
    query.prepare("SELECT course_id FROM course_participants WHERE user_id = :userId");
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса к course_participants:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить данные о курсах.");
        return;
    }

    QList<int> courseIds;
    while (query.next()) {
        courseIds.append(query.value("course_id").toInt());
    }

    if (courseIds.isEmpty()) {
        QLabel *noCoursesLabel = new QLabel("У вас пока нет курсов.", this);
        layout->addWidget(noCoursesLabel);
        return;
    }

    // Формируем запрос для получения данных о курсах
    QStringList placeholders;
    for (int i = 0; i < courseIds.size(); ++i) {
        placeholders << QString(":%1").arg(i);
    }
    QString courseQueryStr = QString("SELECT * FROM courses WHERE course_id IN (%1)").arg(placeholders.join(", "));
    QSqlQuery courseQuery;
    courseQuery.prepare(courseQueryStr);

    for (int i = 0; i < courseIds.size(); ++i) {
        courseQuery.bindValue(QString(":%1").arg(i), courseIds.at(i));
    }

    if (!courseQuery.exec()) {
        qDebug() << "Ошибка выполнения запроса к courses:" << courseQuery.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить данные о курсах.");
        return;
    }

    QLabel *header = new QLabel("Ваши курсы:");
    layout->addWidget(header);

    QListWidget *courseList = new QListWidget(this);
    bool hasCourses = false;

    // Карта для связывания элемента списка с ID курса
    QMap<QListWidgetItem *, int> courseMap;

    while (courseQuery.next()) {
        hasCourses = true;
        QString courseName = courseQuery.value("name").toString();
        int courseId = courseQuery.value("course_id").toInt();

        // Создаем элемент списка
        QListWidgetItem *item = new QListWidgetItem(courseName, courseList);
        courseMap[item] = courseId; // Связываем элемент списка с ID курса
    }

    if (hasCourses) {
        layout->addWidget(courseList);

        // Обработка клика по элементу списка
        connect(courseList, &QListWidget::itemClicked, [courseMap, this](QListWidgetItem *item) {
            if (courseMap.contains(item)) {
                int courseId = courseMap[item];
                qDebug() << "Курс выбран. ID:" << courseId;

                // Создаем файл TargetCourse.txt, если его нет, и записываем ID курса
                QFile file("TargetCourse.txt");
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream out(&file);
                    out << courseId;
                    file.close();
                } else {
                    qDebug() << "Ошибка создания файла TargetCourse.txt";
                    QMessageBox::critical(this, "Ошибка", "Не удалось записать данные о курсе.");
                    return;
                }

                stack->setCurrentIndex(6);
            } else {
                qDebug() << "Элемент списка не связан с курсом.";
            }
        });
    } else {
        QLabel *noCoursesLabel = new QLabel("У вас пока нет курсов.", this);
        layout->addWidget(noCoursesLabel);
    }
}

void Course::loadMentorPage(QVBoxLayout *layout, int userId) {
    QLabel *header = new QLabel("Ваши курсы:");
    layout->addWidget(header);

    QSqlQuery query;
    query.prepare("SELECT * FROM courses WHERE creator_id = :userId");
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить курсы.");
        return;
    }

    QListWidget *courseList = new QListWidget(this);
    bool hasCourses = false;

    // Сохраняем ID курсов для связи с элементами списка
    QMap<QListWidgetItem *, int> courseMap;

    while (query.next()) {
        hasCourses = true;
        QString courseName = query.value("name").toString();
        int courseId = query.value("course_id").toInt(); // ID курса

        // Создаем элемент списка
        QListWidgetItem *item = new QListWidgetItem(courseName, courseList);
        courseMap[item] = courseId; // Связываем элемент списка с ID курса
    }

    if (hasCourses) {
        layout->addWidget(courseList);

        // Обработка клика по элементу списка
        connect(courseList, &QListWidget::itemClicked, [courseMap, this](QListWidgetItem *item) {
            if (courseMap.contains(item)) {
                int courseId = courseMap[item];
                qDebug() << "Курс выбран. ID:" << courseId;

                // Сохраняем ID выбранного курса в файл TargetCourse.txt
                QFile file("TargetCourse.txt");
                if (file.open(QIODevice::WriteOnly)) {
                    QTextStream out(&file);
                    out << courseId;
                    file.close();
                }

                // Переход на страницу с курсом
                stack->setCurrentIndex(6); // Предполагается, что 6 - это индекс страницы CourseList
            } else {
                qDebug() << "Элемент списка не связан с курсом.";
            }
        });
    } else {
        QLabel *noCoursesLabel = new QLabel("Вы еще не создали ни одного курса.", this);
        layout->addWidget(noCoursesLabel);
    }

    // Кнопка для создания курса
    QPushButton *createCourseButton = new QPushButton("Создать курс", this);
    layout->addWidget(createCourseButton);

    connect(createCourseButton, &QPushButton::clicked, [this, userId]() {
        QString university = db.getUserData(userId).toMap()["university"].toString();
        createCourseForm(userId, university);
    });
}


void Course::loadManagerPage(QVBoxLayout *layout, int userId) {
    QLabel *userHeader = new QLabel("Пользователи из вашего университета:");
    layout->addWidget(userHeader);

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE university = (SELECT university FROM users WHERE id = :id)");
    query.bindValue(":id", userId);

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить пользователей.");
        return;
    }

    QListWidget *userList = new QListWidget(this);
    while (query.next()) {
        QString userInfo = query.value("name").toString() + " " +
                           query.value("surname").toString() + " (" +
                           query.value("role").toString() + ")";
        userList->addItem(userInfo);
    }
    layout->addWidget(userList);
}

void Course::loadAdminPage(QVBoxLayout *layout) {
    QLabel *userHeader = new QLabel("Все пользователи:");
    layout->addWidget(userHeader);

    QSqlQuery userQuery("SELECT * FROM users");

    if (!userQuery.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << userQuery.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить пользователей.");
        return;
    }

    QListWidget *userList = new QListWidget(this);

    while (userQuery.next()) {
        QString userInfo = userQuery.value("name").toString() + " " +
                           userQuery.value("surname").toString() + " (" +
                           userQuery.value("role").toString() + ")";
        userList->addItem(userInfo);
    }
    layout->addWidget(userList);
}

void Course::createCourseForm(int userId, const QString &university) {
    QWidget *formPage = new QWidget();
    QVBoxLayout *formLayout = new QVBoxLayout(formPage);

    // Поле для названия курса
    QLineEdit *courseNameEdit = new QLineEdit();
    courseNameEdit->setPlaceholderText("Название курса");
    formLayout->addWidget(courseNameEdit);

    // Поле для описания курса
    QLineEdit *courseDescriptionEdit = new QLineEdit();
    courseDescriptionEdit->setPlaceholderText("Описание курса");
    formLayout->addWidget(courseDescriptionEdit);

    // Поле для направления
    QLineEdit *courseNapravlenieEdit = new QLineEdit();
    courseNapravlenieEdit->setPlaceholderText("Направление курса");
    formLayout->addWidget(courseNapravlenieEdit);

    // Кнопка для отправки формы
    QPushButton *submitButton = new QPushButton("Создать курс");
    formLayout->addWidget(submitButton);

    connect(submitButton, &QPushButton::clicked, [this, courseNameEdit, courseDescriptionEdit, courseNapravlenieEdit, userId]() {
        QString name = courseNameEdit->text();
        QString description = courseDescriptionEdit->text();
        QString napravlenie = courseNapravlenieEdit->text();

        // Проверка на заполнение полей
        if (name.isEmpty() || description.isEmpty() || napravlenie.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля.");
            return;
        }

        // Запрос к базе данных для добавления курса
        QSqlQuery query;
        query.prepare("INSERT INTO courses (name, description, creator_id, datacreate, napravlenie) "
                      "VALUES (:name, :description, :creator_id, :datacreate, :napravlenie)");
        query.bindValue(":name", name);
        query.bindValue(":description", description);
        query.bindValue(":creator_id", userId);
        query.bindValue(":datacreate", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
        query.bindValue(":napravlenie", napravlenie);

        if (query.exec()) {
            QMessageBox::information(this, "Успех", "Курс успешно создан!");

            // Переход на страницу списка курсов
            stack->setCurrentIndex(5);

            // Обновляем только список курсов
            QWidget *currentPage = stack->currentWidget();
            QVBoxLayout *layout = dynamic_cast<QVBoxLayout *>(currentPage->layout());
            if (layout) {
                QScrollArea *scrollArea = currentPage->findChild<QScrollArea *>();
                if (scrollArea) {
                    QWidget *contentWidget = scrollArea->widget();
                    if (contentWidget) {
                        QVBoxLayout *scrollLayout = dynamic_cast<QVBoxLayout *>(contentWidget->layout());
                        if (scrollLayout) {
                            // Удаляем старое содержимое списка
                            QLayoutItem *item;
                            while ((item = scrollLayout->takeAt(0)) != nullptr) {
                                delete item->widget();
                                delete item;
                            }

                            // Перезагружаем список курсов
                            loadMentorPage(scrollLayout, userId);
                        }
                    }
                }
            }

            TemporaryData::clearData();
            qDebug() << "Курс успешно добавлен и список обновлён.";
        } else {
            qDebug() << "Ошибка добавления курса:" << query.lastError().text();
            QMessageBox::warning(this, "Ошибка", "Не удалось создать курс.");
        }
    });



    stack->addWidget(formPage);
    stack->setCurrentWidget(formPage);
}

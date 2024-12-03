#include "CourseList.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QFile>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QLineEdit>
#include <QFileDialog>
#include <QUuid>
#include <QTimer>
#include <QFileSystemWatcher> // Для отслеживания изменений файла

CourseList::CourseList(QStackedWidget *stack, Database *db, QWidget *parent)
    : QWidget(parent), stack(stack), db(db) {

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Создаём наблюдатель за изменениями файла
    QFileSystemWatcher *fileWatcher = new QFileSystemWatcher(this);
    QString targetCourseFile = "TargetCourse.txt";
    fileWatcher->addPath(targetCourseFile);

    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, [this, targetCourseFile]() {
        updateCourseFromFile(targetCourseFile);
    });

    // Загрузка курса при инициализации
    updateCourseFromFile(targetCourseFile);

    // Кнопка назад
    QPushButton *backButton = new QPushButton("Назад");
    connect(backButton, &QPushButton::clicked, [this]() {
        this->stack->setCurrentIndex(5); // Индекс страницы, куда нужно вернуться
    });
    mainLayout->addWidget(backButton);
}

void CourseList::updateCourseFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Ошибка: не удалось открыть" << filePath;
        return;
    }

    QTextStream in(&file);
    QString courseIdStr = in.readLine();
    file.close();

    bool ok;
    int courseId = courseIdStr.toInt(&ok);
    if (!ok) {
        qDebug() << "Ошибка: некорректный ID курса";
        return;
    }

    // Чтение роли пользователя
    int userId = -1;
    QFile userFile("cash.txt");
    if (userFile.open(QIODevice::ReadOnly)) {
        QTextStream userStream(&userFile);
        userId = userStream.readLine().toInt(&ok);
        userFile.close();
    }

    if (userId == -1 || !ok) {
        qDebug() << "Ошибка: некорректный ID пользователя";
        return;
    }

    QString userRole;
    QSqlQuery roleQuery;
    roleQuery.prepare("SELECT role FROM users WHERE id = :id");
    roleQuery.bindValue(":id", userId);
    if (roleQuery.exec() && roleQuery.next()) {
        userRole = roleQuery.value("role").toString();
    } else {
        qDebug() << "Ошибка получения роли пользователя:" << roleQuery.lastError().text();
        return;
    }

    // Обновляем содержимое курса
    loadCourseContent(courseId, userRole);
}

void CourseList::loadCourseContent(int courseId, const QString &userRole) {
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout *>(this->layout());
    if (!mainLayout) {
        qDebug() << "Ошибка: основной макет не установлен";
        return;
    }

    // Удаляем старые виджеты перед загрузкой новых данных
    clearOldContent();

    QSqlQuery query;
    query.prepare("SELECT * FROM courses WHERE course_id = :id");
    query.bindValue(":id", courseId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Ошибка загрузки данных курса:" << query.lastError().text();
        QLabel *errorLabel = new QLabel("Не удалось загрузить курс.", this);
        mainLayout->addWidget(errorLabel);
        return;
    }

    QString courseName = query.value("name").toString();
    QString courseDescription = query.value("description").toString();
    QString courseDirection = query.value("napravlenie").toString();
    QString courseDate = query.value("datacreate").toString();

    QPushButton *Course = new QPushButton("Главная страница", this);
    mainLayout->addWidget(Course);

    connect(Course, &QPushButton::clicked, [this]() {
        this->stack->setCurrentIndex(5);  // Переход на страницу 1
    });

    QLabel *header = new QLabel("Информация о курсе");
    header->setStyleSheet("font-weight: bold; font-size: 18px;");
    mainLayout->addWidget(header);

    mainLayout->addWidget(new QLabel("Название: " + courseName));
    mainLayout->addWidget(new QLabel("Описание: " + courseDescription));
    mainLayout->addWidget(new QLabel("Направление: " + courseDirection));
    mainLayout->addWidget(new QLabel("Дата создания: " + courseDate));

    // Загружаем участников курса
    loadCourseParticipants(courseId, userRole);

    // Загружаем задания курса
    loadCourseTasks(courseId);
}
void CourseList::clearOldContent() {
    // Удаляем старые виджеты, чтобы избежать дублирования
    QList<QWidget *> widgets = findChildren<QWidget *>();
    for (QWidget *widget : widgets) {
        widget->deleteLater();
    }
}

void CourseList::loadCourseTasks(int courseId) {
    // Удаляем старый список заданий, если он уже существует
    QListWidget *existingTaskList = findChild<QListWidget *>("taskList");
    if (existingTaskList) {
        delete existingTaskList;
    }

    // Создаем новый список заданий
    QSqlQuery query;
    query.prepare("SELECT * FROM lessons WHERE course_id = :courseId");
    query.bindValue(":courseId", courseId);

    QListWidget *taskList = new QListWidget(this);
    taskList->setObjectName("taskList"); // Уникальное имя для поиска
    if (!query.exec()) {
        qDebug() << "Ошибка загрузки заданий:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString taskId = query.value("lesson_id").toString();
        QString taskName = query.value("name").toString();
        QString taskDescription = query.value("description").toString();

        QListWidgetItem *item = new QListWidgetItem(taskName, taskList);
        item->setToolTip(taskDescription);

        // Сохраняем ID задания в data для использования при клике
        item->setData(Qt::UserRole, taskId);
    }

    layout()->addWidget(taskList); // Добавляем список в макет

    connect(taskList, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        // Получаем ID задания из data
        QString taskId = item->data(Qt::UserRole).toString();

        // Запись выбранного задания в файл
        QFile file("TargetLesson.txt");
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream out(&file);
            out << taskId; // Сохраняем ID задания
            file.close();
        }

        // Переход на страницу с заданием
        stack->setCurrentIndex(7); // Индекс страницы с детальной информацией о задании
    });
}

void CourseList::loadCourseParticipants(int courseId, const QString &userRole) {
    // Удаляем старые виджеты, если они существуют
    QListWidget *existingParticipantsList = findChild<QListWidget *>("participantsList");
    QListWidget *existingUsersList = findChild<QListWidget *>("usersList");
    QLabel *existingParticipantsLabel = findChild<QLabel *>("participantsLabel");
    QLabel *existingUsersLabel = findChild<QLabel *>("usersLabel");

    if (existingParticipantsList) {
        delete existingParticipantsList;
    }
    if (existingUsersList) {
        delete existingUsersList;
    }
    if (existingParticipantsLabel) {
        delete existingParticipantsLabel;
    }
    if (existingUsersLabel) {
        delete existingUsersLabel;
    }

    // Проверяем наличие основного макета
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout *>(this->layout());
    if (!mainLayout) {
        qDebug() << "Ошибка: основной макет не установлен";
        return;
    }

    // Список участников курса
    QLabel *participantsLabel = new QLabel("Участники курса:");
    participantsLabel->setObjectName("participantsLabel");
    mainLayout->addWidget(participantsLabel);

    QListWidget *participantsList = new QListWidget(this);
    participantsList->setObjectName("participantsList");
    mainLayout->addWidget(participantsList);

    QSqlQuery participantsQuery;
    participantsQuery.prepare("SELECT u.id, u.name FROM users u "
                              "JOIN course_participants cp ON u.id = cp.user_id "
                              "WHERE cp.course_id = :courseId");
    participantsQuery.bindValue(":courseId", courseId);

    if (participantsQuery.exec()) {
        while (participantsQuery.next()) {
            QString userId = participantsQuery.value("id").toString();
            QString userName = participantsQuery.value("name").toString();

            QListWidgetItem *item = new QListWidgetItem(userName, participantsList);
            item->setData(Qt::UserRole, userId);
        }
    } else {
        qDebug() << "Ошибка загрузки списка участников курса:" << participantsQuery.lastError().text();
    }

    // Удаление участников доступно только для пользователей с ролью, отличной от "Обучающийся"
    if (userRole != "Обучающийся") {
        connect(participantsList, &QListWidget::itemClicked, [this,userRole, courseId](QListWidgetItem *item) {
            QString userId = item->data(Qt::UserRole).toString();

            QSqlQuery query;
            query.prepare("DELETE FROM course_participants WHERE course_id = :courseId AND user_id = :userId");
            query.bindValue(":courseId", courseId);
            query.bindValue(":userId", userId);

            if (query.exec()) {
                QMessageBox::information(this, "Успех", "Пользователь удален из курса!");
                loadCourseParticipants(courseId, userRole); // Обновляем список участников
            } else {
                QMessageBox::critical(this, "Ошибка", "Не удалось удалить пользователя из курса: " + query.lastError().text());
            }
        });
    }

    // Список всех доступных пользователей
    QLabel *usersLabel = new QLabel("Доступные пользователи:");
    usersLabel->setObjectName("usersLabel");
    mainLayout->addWidget(usersLabel);

    QListWidget *usersList = new QListWidget(this);
    usersList->setObjectName("usersList");
    mainLayout->addWidget(usersList);

    QSqlQuery usersQuery;
    usersQuery.prepare("SELECT id, name FROM users WHERE id NOT IN "
                       "(SELECT user_id FROM course_participants WHERE course_id = :courseId)");
    usersQuery.bindValue(":courseId", courseId);

    if (usersQuery.exec()) {
        while (usersQuery.next()) {
            QString userId = usersQuery.value("id").toString();
            QString userName = usersQuery.value("name").toString();

            QListWidgetItem *item = new QListWidgetItem(userName, usersList);
            item->setData(Qt::UserRole, userId);
        }
    } else {
        qDebug() << "Ошибка загрузки списка доступных пользователей:" << usersQuery.lastError().text();
    }

    // Добавление пользователя в курс
    connect(usersList, &QListWidget::itemClicked, [this, userRole, courseId, participantsList, usersList](QListWidgetItem *item) {
        QString userId = item->data(Qt::UserRole).toString();

        QSqlQuery query;
        query.prepare("INSERT INTO course_participants (course_id, user_id) VALUES (:courseId, :userId)");
        query.bindValue(":courseId", courseId);
        query.bindValue(":userId", userId);

        if (query.exec()) {
            QMessageBox::information(this, "Успех", "Пользователь добавлен в курс!");

            // Обновляем оба списка
            loadCourseParticipants(courseId, userRole);
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось добавить пользователя в курс: " + query.lastError().text());
        }
    });
}


void CourseList::createTaskForm(int courseId) {
    QWidget *formPage = new QWidget();
    QVBoxLayout *formLayout = new QVBoxLayout(formPage);

    QLineEdit *taskNameEdit = new QLineEdit();  // Это строка теперь корректна
    taskNameEdit->setPlaceholderText("Название задания");
    formLayout->addWidget(taskNameEdit);

    QLineEdit *taskDescriptionEdit = new QLineEdit();
    taskDescriptionEdit->setPlaceholderText("Описание задания");
    formLayout->addWidget(taskDescriptionEdit);

    QPushButton *submitButton = new QPushButton("Создать задание");
    formLayout->addWidget(submitButton);

    connect(submitButton, &QPushButton::clicked, [this, taskNameEdit, taskDescriptionEdit, courseId, formPage]() {
        QString name = taskNameEdit->text();
        QString description = taskDescriptionEdit->text();

        if (name.isEmpty() || description.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Заполните все поля.");
            return;
        }

        QSqlQuery query;
        query.prepare("INSERT INTO lessons (name, description, course_id) VALUES (:name, :description, :course_id)");
        query.bindValue(":name", name);
        query.bindValue(":description", description);
        query.bindValue(":course_id", courseId);

        if (query.exec()) {
            QMessageBox::information(this, "Успех", "Задание создано!");

            // Обновление списка заданий на странице курса
            loadCourseTasks(courseId);

            // Возвращаемся на страницу курса
            stack->setCurrentIndex(5);  // Индекс страницы с курсом (возможно, нужно корректировать)
        } else {
            qDebug() << "Ошибка создания задания:" << query.lastError().text();
        }

        // Закрыть форму создания задания (если нужно)
        stack->removeWidget(formPage);
    });

    stack->addWidget(formPage);  // Добавляем форму в стек
    stack->setCurrentWidget(formPage);  // Переключаемся на форму
}
// Реализация добавления пользователя в курс
void CourseList::addUserToCourse(int userId, int courseId)
{
    QSqlQuery query;
    query.prepare("INSERT INTO course_participants (user_id, course_id) VALUES (:user_id, :course_id)");
    query.bindValue(":user_id", userId);
    query.bindValue(":course_id", courseId);

    if (query.exec()) {
        qDebug() << "Пользователь успешно добавлен в курс.";
        // Дополнительно можно обновить UI, если нужно
    } else {
        qDebug() << "Ошибка добавления пользователя в курс:" << query.lastError().text();
    }
}

// Реализация удаления пользователя из курса
void CourseList::removeUserFromCourse(int userId, int courseId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM course_participants WHERE user_id = :user_id AND course_id = :course_id");
    query.bindValue(":user_id", userId);
    query.bindValue(":course_id", courseId);

    if (query.exec()) {
        qDebug() << "Пользователь успешно удален из курса.";
        // Дополнительно можно обновить UI, если нужно
    } else {
        qDebug() << "Ошибка удаления пользователя из курса:" << query.lastError().text();
    }
}
void CourseList::addUserToCourseAsync(int userId, int courseId)
{
    // Используем QtConcurrent для асинхронного выполнения
    QtConcurrent::run([this, userId, courseId]() {
        addUserToCourse(userId, courseId);
    });
}

// Асинхронное удаление пользователя из курса
void CourseList::removeUserFromCourseAsync(int userId, int courseId)
{
    // Используем QtConcurrent для асинхронного выполнения
    QtConcurrent::run([this, userId, courseId]() {
        removeUserFromCourse(userId, courseId);
    });
}

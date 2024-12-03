#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

Database::Database() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("app_data.db"); // Общая база данных
}

Database::~Database() {
    if (db.isOpen()) {
        db.close();
    }
}

bool Database::open() {
    if (!db.open()) {
        qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
        return false;
    }

    if (!createTables()) {
        qDebug() << "Error creating tables.";
        return false;
    }

    return true;
}

void Database::close() {
    db.close();
}

bool Database::createTables() {
    QSqlQuery query;

    bool success = query.exec("CREATE TABLE IF NOT EXISTS users ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "login TEXT UNIQUE, "
                              "password TEXT, "
                              "name TEXT, "
                              "surname TEXT, "
                              "university TEXT, "
                              "birth_date TEXT, "
                              "gender TEXT, "
                              "role TEXT)");
    if (!success) {
        qDebug() << "Error creating users table: " << query.lastError();
        return false;
    }

    success = query.exec("CREATE TABLE IF NOT EXISTS courses ("
                         "course_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "name TEXT, "
                         "description TEXT, "
                         "creator_id INTEGER, "
                         "datacreate TEXT, "
                         "napravlenie TEXT, "
                         "members TEXT, "
                         "lessons TEXT)");
    if (!success) {
        qDebug() << "Error creating courses table: " << query.lastError();
        return false;
    }

    success = query.exec("CREATE TABLE IF NOT EXISTS lessons ("
                         "lesson_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "name TEXT, "
                         "description TEXT, "
                         "datacreate TEXT, "
                         "sources TEXT, "
                         "time TEXT, "
                         "course_id INTEGER)");
    if (!success) {
        qDebug() << "Error creating lessons table: " << query.lastError();
        return false;
    }

    success = query.exec("CREATE TABLE IF NOT EXISTS user_lessons_flags ("
                         "user_id INTEGER, "
                         "lesson_id INTEGER, "
                         "flag TEXT, "
                         "PRIMARY KEY (user_id, lesson_id))");
    if (!success) {
        qDebug() << "Error creating user_lessons_flags table: " << query.lastError();
        return false;
    }

    // Добавление таблицы course_participants
    success = query.exec("CREATE TABLE IF NOT EXISTS course_participants ("
                         "course_id INTEGER NOT NULL, "
                         "user_id INTEGER NOT NULL, "
                         "PRIMARY KEY (course_id, user_id))");
    if (!success) {
        qDebug() << "Error creating course_participants table: " << query.lastError();
        return false;
    }
    // Добавление таблицы answers
    success = query.exec("CREATE TABLE IF NOT EXISTS answers ("
                         "answer_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "user_id INTEGER NOT NULL, "
                         "lesson_id INTEGER NOT NULL, "
                         "sources TEXT)");
    if (!success) {
        qDebug() << "Error creating answers table: " << query.lastError();
        return false;
    }
    // Добавление таблицы answers
    success = query.exec("CREATE TABLE IF NOT EXISTS comments ("
                         "comments_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "userfirst_id INTEGER NOT NULL, "
                         "usersecond_id INTEGER NOT NULL, "
                         "message TEXT,"
                         "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)");
    if (!success) {
        qDebug() << "Error creating answers table: " << query.lastError();
        return false;
    }
    return true;
}

// Методы для работы с таблицей пользователей
int Database::checkLoginCredentials(const QString &login, const QString &password) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE login = :login AND password = :password");
    query.bindValue(":login", login);
    query.bindValue(":password", password);

    if (query.exec() && query.next()) {
        return query.value(0).toInt(); // Возвращаем ID пользователя, если логин и пароль совпали
    }
    return -1; // Неверный логин или пароль
}

int Database::addUser(const QString& login, const QString& password, const QString& name, const QString& surname,
                      const QString& university, const QString& birthDate, const QString& gender, const QString& role) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (login, password, name, surname, university, birth_date, gender, role) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(login);
    query.addBindValue(password);
    query.addBindValue(name);
    query.addBindValue(surname);
    query.addBindValue(university);
    query.addBindValue(birthDate);
    query.addBindValue(gender);
    query.addBindValue(role);

    if (query.exec()) {
        return query.lastInsertId().toInt();
    } else {
        qDebug() << "Error inserting user: " << query.lastError();
        return -1;
    }
}

bool Database::checkUserExist(const QString& login) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE login = ?");
    query.addBindValue(login);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

QVariant Database::getUserData(int userId) {
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE id = :id");
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        QVariantMap userData;

        for (int i = 0; i < record.count(); ++i) {
            userData[record.fieldName(i)] = record.value(i);
        }

        return userData; // Возвращаем QVariantMap
    }

    return QVariant(); // Возвращаем пустой QVariant в случае ошибки
}

// Методы для работы с курсами
QVariantList Database::getCoursesByUserId(int userId) {
    QSqlQuery query;
    query.prepare("SELECT * FROM courses WHERE members LIKE :userId");
    query.bindValue(":userId", "%" + QString::number(userId) + "%");

    if (query.exec()) {
        QVariantList courses;
        while (query.next()) {
            QVariantMap course;
            course["course_id"] = query.value("course_id");
            course["name"] = query.value("name");
            course["description"] = query.value("description");
            course["datacreate"] = query.value("datacreate");
            courses.append(course);
        }
        return courses;
    }
    return QVariantList(); // Если курсы не найдены
}

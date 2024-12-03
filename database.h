#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QVariant>
#include <QVariantList>
#include <QString>

class Database {
public:
    // Конструктор и деструктор
    Database();
    ~Database();

    // Методы для управления подключением к базе данных
    bool open();
    void close();

    // Методы для работы с таблицами
    bool createTables();

    // Методы для работы с пользователями
    int checkLoginCredentials(const QString &login, const QString &password);
    int addUser(const QString& login, const QString& password, const QString& name,
                const QString& surname, const QString& university,
                const QString& birthDate, const QString& gender, const QString& role);
    bool checkUserExist(const QString& login);
    QVariant getUserData(int userId);

    // Методы для работы с курсами
    QVariantList getCoursesByUserId(int userId);

private:
    QSqlDatabase db; // Экземпляр базы данных

};

#endif // DATABASE_H

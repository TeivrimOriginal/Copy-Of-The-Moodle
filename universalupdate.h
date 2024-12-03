#ifndef UNIVERSALUPDATE_H
#define UNIVERSALUPDATE_H

#include <QObject>
#include <QTimer>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <functional>

class UniversalUpdate : public QObject {
    Q_OBJECT

public:
    // Конструктор: принимает родительский объект и интервал обновления (в миллисекундах)
    explicit UniversalUpdate(QObject *parent = nullptr, int intervalMs = 5000);

    // Метод для установки SQL-запроса и обработки результата
    void setUpdateQuery(const QString &query, std::function<void(QSqlQuery &)> onResultCallback);

private slots:
    // Слот для обновления данных
    void updateData();

private:
    QTimer *updateTimer;   // Таймер для регулярного обновления
    QString queryText;     // Хранит SQL-запрос
    std::function<void(QSqlQuery &)> resultCallback; // Обработчик результата запроса
};

#endif // UNIVERSALUPDATE_H

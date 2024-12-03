#include <QTimer>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <UniversalUpdate.h>
class UniversalUpdate : public QObject {
    Q_OBJECT

public:
    explicit UniversalUpdate(QObject *parent = nullptr, int intervalMs = 5000);

    // Метод для установки SQL-запроса и обработки результата
    void setUpdateQuery(const QString &query, std::function<void(QSqlQuery &)> onResultCallback);

private slots:
    void updateData(); // Слот для обновления данных

private:
    QTimer *updateTimer; // Таймер для обновлений
    QString queryText;   // SQL-запрос
    std::function<void(QSqlQuery &)> resultCallback; // Обработчик результата
};

// Реализация конструктора
UniversalUpdate::UniversalUpdate(QObject *parent, int intervalMs)
    : QObject(parent), updateTimer(new QTimer(this)) {
    connect(updateTimer, &QTimer::timeout, this, &UniversalUpdate::updateData);
    updateTimer->start(intervalMs); // Запуск таймера с указанным интервалом
}

// Установка SQL-запроса и обработчика результата
void UniversalUpdate::setUpdateQuery(const QString &query, std::function<void(QSqlQuery &)> onResultCallback) {
    queryText = query;
    resultCallback = onResultCallback;
}

// Реализация метода обновления данных
void UniversalUpdate::updateData() {
    if (queryText.isEmpty() || !resultCallback) {
        qDebug() << "Запрос или обработчик результата не установлены.";
        return;
    }

    QSqlQuery query;
    if (query.exec(queryText)) {
        resultCallback(query); // Вызываем обработчик результата
    } else {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
    }
}

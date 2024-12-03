// TemporaryData.h
#ifndef TEMPORARYDATA_H
#define TEMPORARYDATA_H

#include <QMap>
#include <QString>

class TemporaryData {
public:
    static void updateData(const QString& key, const QString& newValue);  // Функция обновления данных
    static QString getData(const QString& key);  // Функция получения данных
    static void clearData();  // Очистка всех временных данных (при необходимости)

private:
    static QMap<QString, QString> tempData;  // Содержит все временные данные
};

#endif // TEMPORARYDATA_H

// TemporaryData.cpp
#include "TemporaryData.h"

// Инициализация статической переменной для хранения данных
QMap<QString, QString> TemporaryData::tempData;

// Функция обновления временных данных
void TemporaryData::updateData(const QString& key, const QString& newValue) {
    // Обновляем данные по ключу
    tempData[key] = newValue;
}

// Функция получения данных по ключу
QString TemporaryData::getData(const QString& key) {
    if (tempData.contains(key)) {
        return tempData[key];
    }
    return "";  // Если данных нет, возвращаем пустую строку
}

// Функция очистки всех данных (можно использовать, если нужно полностью удалить временные данные)
void TemporaryData::clearData() {
    tempData.clear();
}

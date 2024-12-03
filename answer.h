#ifndef ANSWER_H
#define ANSWER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include "database.h"

class Answer : public QWidget
{
    Q_OBJECT

public:
    explicit Answer(QWidget *parent = nullptr, Database *db = nullptr);
    ~Answer();

private:
    QVBoxLayout *layout; // Основной макет
    Database *db;        // Указатель на базу данных

    void loadAnswerData(); // Загрузка данных ответа
    void displayTaskData(int lessonId); // Отображение данных задания
    void refreshUI();     // Обновление интерфейса

    int getUserId();      // Получение ID текущего пользователя
    QString getUserType(); // Получение типа текущего пользователя
};

#endif // ANSWER_H

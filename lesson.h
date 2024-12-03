#ifndef LESSON_H
#define LESSON_H

#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QString>
#include "Database.h"

class Lesson : public QWidget
{
    Q_OBJECT

public:
    explicit Lesson(QStackedWidget *stack, Database *db, QWidget *parent = nullptr);
    ~Lesson();

    void loadTaskData(int taskId);                  // Загрузка данных задания
    void addFileToDatabase(int taskId, int courseId); // Добавление файла в базу данных
    void viewFiles(int taskId, int courseId);                     // Просмотр файлов
    void addAnswer(int taskId);                     // Добавление ответа
    void loadAnswers(int taskId);                   // Загрузка ответов
    int getUserId();                                // Получение ID пользователя
    void saveAnswer(int answerId);                  // Сохранение выбранного ответа

public slots:
    void onAddFileButtonClicked(int taskId, int courseId); // Слот для добавления файла
    void onUpdateTaskButtonClicked();                     // Слот для обновления задания

private:
    void updateTaskFromFile(const QString &filePath); // Обновление задания из файла
    void clearCurrentTaskData();                      // Очистка текущих данных UI

    QStackedWidget *stack; // Стек для переключения между страницами
    Database *db;          // База данных
    int taskId;            // Текущий ID задания
};

#endif // LESSON_H

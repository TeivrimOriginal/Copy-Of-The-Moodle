#ifndef COURSELIST_H
#define COURSELIST_H

#include <QWidget>
#include <QStackedWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include <QtConcurrent>

class Database;  // Предполагаем, что это класс для работы с базой данных

class CourseList : public QWidget
{
    Q_OBJECT

public:
    explicit CourseList(QStackedWidget *stack, Database *db, QWidget *parent = nullptr);

private:
    QStackedWidget *stack;
    Database *db;
    void updateCourseFromFile(const QString &filePath);

    void loadCourseContent(int courseId, const QString &userRole);
    void clearOldContent();
    void loadCourseTasks(int courseId);
    void loadCourseParticipants(int courseId, const QString &userRole);
    void createTaskForm(int courseId);
    void addUserToCourse(int userId, int courseId);  // Метод для добавления пользователя в курс
    void removeUserFromCourse(int userId, int courseId);  // Метод для удаления пользователя из курса

    // Асинхронные версии методов
    void addUserToCourseAsync(int userId, int courseId);
    void removeUserFromCourseAsync(int userId, int courseId);
};

#endif // COURSELIST_H

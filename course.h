#ifndef COURSE_H
#define COURSE_H

#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QListWidget>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QLineEdit>
#include "database.h"

class Course : public QWidget
{
    Q_OBJECT

public:
    explicit Course(QStackedWidget *stack, Database &db);  // Removed QWidget *parent
    ~Course();

    bool open();
    void close();

private:
    QStackedWidget *stack;
    Database &db;
    int getUserIdFromCash();
    QString getUserRole(int userId);
    void loadUserPage(int userId, QVBoxLayout *layout);
    void loadStudentPage(QVBoxLayout *layout, int userId);
    void loadMentorPage(QVBoxLayout *layout, int userId);
    void loadManagerPage(QVBoxLayout *layout, int userId);
    void loadAdminPage(QVBoxLayout *layout);
    void createCourseForm(int userId, const QString &university);

};

#endif // COURSE_H

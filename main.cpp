#include <QApplication>
#include <QStackedWidget>
#include "page1.h"
#include "page2.h"
#include "page3.h"
#include "pagelogin.h"
#include "database.h"
#include "pageUserType.h"
#include "course.h"
#include "courselist.h"
#include "answer.h"
#include "lesson.h"
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QStackedWidget stack;
    Database db;

    if (!db.open()) {
        return -1;
    }

    db.createTables();

    Page1 page1(&stack, db);  // 0
    Page2 page2(&stack, db);
    Page3 page3(&stack, db); // 3
    PageLogin pageLogin(&stack, db);
    PageUserType pageUserType(&stack, db); // 4
    Course course(&stack, db);
    CourseList courselist(&stack, &db); // 6
    Lesson lesson(&stack, &db); // 7
    Answer answer(&stack, &db); // 8

    stack.addWidget(&page1);
    stack.addWidget(&page2);
    stack.addWidget(&page3);
    stack.addWidget(&pageLogin);
    stack.addWidget(&pageUserType);
    stack.addWidget(&course);
    stack.addWidget(&courselist);
    stack.addWidget(&lesson);
    stack.addWidget(&answer);

    stack.setCurrentIndex(0);  // Начинаем с первой страницы

    stack.setWindowTitle("Система регистрации и входа");
    stack.resize(400, 300);
    stack.show();

    return app.exec();
}

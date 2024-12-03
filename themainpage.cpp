#include "themainpage.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QCoreApplication>
#include <QProcess>
#include <QApplication>

THEMAINPAGE::THEMAINPAGE(QStackedWidget *stack) : stack(stack) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QPushButton *registerButton = new QPushButton("Вернуться назад", this);
        layout->addWidget(registerButton);

        connect(registerButton, &QPushButton::clicked, [stack]() {
            stack->setCurrentIndex(2);  // Переход на страницу 1
        });
}

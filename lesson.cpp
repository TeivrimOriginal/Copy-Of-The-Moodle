#include "Lesson.h"
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QFileDialog>
#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QFile>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QLineEdit>
#include <QFileDialog>
#include <QUuid>
#include <QTimer>  // Для таймера
Lesson::Lesson(QStackedWidget *stack, Database *db, QWidget *parent)
    : QWidget(parent), stack(stack), db(db)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    this->setLayout(mainLayout);

    // Инициализация данных
    updateTaskFromFile("TargetLesson.txt");

    // Кнопка "Назад"
    QPushButton *backButton = new QPushButton("Назад");
    connect(backButton, &QPushButton::clicked, [this]() {
        this->stack->setCurrentIndex(6);
    });
    mainLayout->addWidget(backButton);
}

// Деструктор
Lesson::~Lesson() {
    qDebug() << "Деструктор класса Lesson вызван";
}

// Загрузка данных задания
void Lesson::loadTaskData(int taskId) {
    QSqlQuery query;
    query.prepare("SELECT * FROM lessons WHERE lesson_id = :taskId");
    query.bindValue(":taskId", taskId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Ошибка загрузки данных задания:" << query.lastError().text();
        QLabel *errorLabel = new QLabel("Не удалось загрузить задание.", this);
        layout()->addWidget(errorLabel);
        return;
    }

    QString taskName = query.value("name").toString();
    QString taskDescription = query.value("description").toString();

    QLabel *header = new QLabel("Информация о задании");
    header->setStyleSheet("font-weight: bold; font-size: 18px;");
    layout()->addWidget(header);

    layout()->addWidget(new QLabel("Название: " + taskName));
    layout()->addWidget(new QLabel("Описание: " + taskDescription));
}

// Добавление файла в базу данных
void Lesson::addFileToDatabase(int taskId, int courseId) {
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл для загрузки");

    if (filePath.isEmpty()) {
        qDebug() << "Ошибка: файл не выбран";
        return;
    }

    QString courseDir = QString("Files/lessons/%1/%2/").arg(courseId).arg(taskId);
    QDir dir;
    if (!dir.mkpath(courseDir)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать директорию для сохранения файла.");
        return;
    }

    QString fileName = QFileInfo(filePath).fileName();
    QString destinationPath = courseDir + fileName;
    if (!QFile::copy(filePath, destinationPath)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл.");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT sources FROM lessons WHERE lesson_id = :taskId");
    query.bindValue(":taskId", taskId);
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить текущие источники задания.");
        return;
    }

    QString currentSources = query.value("sources").toString();
    QString updatedSources = currentSources.isEmpty() ? fileName : currentSources + "*, *" + fileName;

    query.prepare("UPDATE lessons SET sources = :sources WHERE lesson_id = :taskId");
    query.bindValue(":sources", updatedSources);
    query.bindValue(":taskId", taskId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось обновить источники в базе данных.");
    } else {
        QMessageBox::information(this, "Успех", "Файл успешно добавлен.");
    }
}

// Просмотр файлов
void Lesson::viewFiles(int taskId, int courseId) {
    QSqlQuery query;
    query.prepare("SELECT sources FROM lessons WHERE lesson_id = :taskId");
    query.bindValue(":taskId", taskId);

    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить файлы.");
        return;
    }

    QString sources = query.value("sources").toString();
    if (sources.isEmpty()) {
        QMessageBox::information(this, "Информация", "Для этого задания нет файлов.");
        return;
    }

    QStringList fileList = sources.split("*, *");
    QString courseDir = QString("Files/lessons/%1/%2/").arg(courseId).arg(taskId);

    QVBoxLayout *fileLayout = new QVBoxLayout();
    for (const QString &fileName : fileList) {
        QString fullPath = courseDir + fileName;

        if (QFile::exists(fullPath)) {
            QPushButton *fileButton = new QPushButton(fileName);
            connect(fileButton, &QPushButton::clicked, [this, fullPath, fileName]() {
                QString savePath = QFileDialog::getSaveFileName(this, "Сохранить файл", fileName);
                if (!savePath.isEmpty()) {
                    if (QFile::copy(fullPath, savePath)) {
                        QMessageBox::information(this, "Успех", "Файл успешно сохранен.");
                    } else {
                        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл.");
                    }
                }
            });
            fileLayout->addWidget(fileButton);
        } else {
            QLabel *fileLabel = new QLabel("Файл " + fileName + " не найден.");
            fileLayout->addWidget(fileLabel);
        }
    }

    QWidget *fileWidget = new QWidget();
    fileWidget->setLayout(fileLayout);
    fileWidget->show();
}

// Добавление ответа (слот)
void Lesson::addAnswer(int taskId) {
    QMessageBox::information(this, "Ответ", "Ответ на задание " + QString::number(taskId) + " добавлен.");
}

int Lesson::getUserId() {
    QFile userFile("cash.txt");
    if (!userFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Ошибка: не удалось открыть TargetUser.txt";
        return -1;
    }

    QTextStream in(&userFile);
    QString userIdStr = in.readLine();
    userFile.close();

    bool ok;
    int userId = userIdStr.toInt(&ok);
    if (!ok) {
        qDebug() << "Ошибка: некорректный ID пользователя";
        return -1;
    }
    return userId;
}




void Lesson::onAddFileButtonClicked(int taskId, int courseId) {
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл для загрузки");

    if (filePath.isEmpty()) {
        qDebug() << "Ошибка: файл не выбран";
        return;
    }

    // Создание структуры папок
    QString courseDir = QString("Files/lessons/%1/%2/").arg(courseId).arg(taskId);
    QDir dir;
    if (!dir.mkpath(courseDir)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать директорию для сохранения файла.");
        return;
    }

    // Сохранение файла
    QString fileName = QFileInfo(filePath).fileName();
    QString destinationPath = courseDir + fileName;
    if (!QFile::copy(filePath, destinationPath)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл.");
        return;
    }

    // Обновление базы данных
    QSqlQuery query;
    query.prepare("SELECT sources FROM lessons WHERE lesson_id = :taskId");
    query.bindValue(":taskId", taskId);
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить текущие источники задания.");
        return;
    }

    QString currentSources = query.value("sources").toString();
    QString updatedSources = currentSources.isEmpty() ? fileName : currentSources + "*, *" + fileName;

    query.prepare("UPDATE lessons SET sources = :sources WHERE lesson_id = :taskId");
    query.bindValue(":sources", updatedSources);
    query.bindValue(":taskId", taskId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось обновить источники в базе данных.");
    } else {
        QMessageBox::information(this, "Успех", "Файл успешно добавлен.");
    }
}
void Lesson::loadAnswers(int taskId) {
    QSqlQuery query;
    query.prepare("SELECT * FROM answers WHERE lesson_id = :taskId");
    query.bindValue(":taskId", taskId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить ответы.");
        return;
    }

    qDebug() << "Answers query executed successfully for task ID:" << taskId;

    QVBoxLayout *answerLayout = new QVBoxLayout();

    bool foundAnswers = false; // Flag to check if answers exist
    while (query.next()) {
        int answerId = query.value("answer_id").toInt();
        QString answerText = query.value("answer_text").toString();

        qDebug() << "Adding answer: " << answerText;

        QPushButton *answerButton = new QPushButton(answerText);
        connect(answerButton, &QPushButton::clicked, [this, answerId]() {
            saveAnswer(answerId);
            this->stack->setCurrentIndex(9); // Pереход в стек 9
        });
        answerLayout->addWidget(answerButton);
        foundAnswers = true;
    }

    if (!foundAnswers) {
        QLabel *noAnswersLabel = new QLabel("Нет доступных ответов для этого задания.");
        answerLayout->addWidget(noAnswersLabel);
    }

    QWidget *answerWidget = new QWidget();
    answerWidget->setLayout(answerLayout);
    layout()->addWidget(answerWidget); // Ensure answers are added to main layout
}

// Сохранение выбранного ответа и ID пользователя
void Lesson::saveAnswer(int answerId) {
    int userId = getUserId();  // Получаем ID пользователя

    QFile answerFile("TargetAnswer.txt");
    if (!answerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Ошибка: не удалось открыть targetAnswer.txt";
        return;
    }

    QTextStream out(&answerFile);
    out << "UserID: " << userId << "\n";
    out << "AnswerID: " << answerId << "\n";
    answerFile.close();
}
void Lesson::updateTaskFromFile(const QString &filePath) {
    QFile lessonFile(filePath);
    if (!lessonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка: не удалось открыть файл" << filePath;
        return;
    }

    QTextStream lessonIn(&lessonFile);
    QString taskIdStr = lessonIn.readLine();
    lessonFile.close();

    bool ok;
    int newTaskId = taskIdStr.toInt(&ok);
    if (!ok) {
        qDebug() << "Ошибка: некорректный ID задания в файле" << filePath;
        return;
    }

    if (newTaskId == this->taskId) {
        qDebug() << "Текущее задание уже обновлено. ID:" << newTaskId;
        return;
    }

    this->taskId = newTaskId;
    qDebug() << "Обновление текущего задания. Новый ID:" << newTaskId;

    // Перезагрузка данных
    clearCurrentTaskData(); // Очистите текущий интерфейс (опционально)
    loadTaskData(newTaskId);
    loadAnswers(newTaskId);
}

// Очистка текущих данных интерфейса
void Lesson::clearCurrentTaskData() {
    QLayout *mainLayout = this->layout();
    while (QLayoutItem *item = mainLayout->takeAt(0)) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}
void Lesson::onUpdateTaskButtonClicked()
{
    // Имя файла, из которого обновляется задание
    QString filePath = "TargetLesson.txt";

    // Проверяем, существует ли файл
    QFile file(filePath);
    if (!file.exists()) {
        QMessageBox::critical(this, "Ошибка", "Файл TargetLesson.txt не найден.");
        return;
    }

    // Обновляем задание из файла
    updateTaskFromFile(filePath);
}


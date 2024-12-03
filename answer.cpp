#include "answer.h"
#include <QDir>

Answer::Answer(QWidget *parent, Database *db)
    : QWidget(parent), db(db)
{
    layout = new QVBoxLayout(this);
    loadAnswerData();
}

Answer::~Answer() {}

void Answer::loadAnswerData() {
    int userId = getUserId();
    QString userType = getUserType();

    QFile answerFile("TargetaAnswer.txt");
    if (!answerFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка открытия файла TargetaAnswer.txt.";
        return;
    }

    QTextStream in(&answerFile);
    int answerId = in.readLine().toInt();
    answerFile.close();

    QSqlQuery query;
    query.prepare("SELECT lesson_id, sources FROM answers WHERE answer_id = :answer_id AND user_id = :user_id");
    query.bindValue(":answer_id", answerId);
    query.bindValue(":user_id", userId);
    if (!query.exec() || !query.next()) {
        qDebug() << "Ответ с таким answer_id не найден.";
        return;
    }

    int lessonId = query.value("lesson_id").toInt();
    QString sources = query.value("sources").toString();

    displayTaskData(lessonId);

    QLabel *taskLabel = new QLabel("Задание: " + QString::number(lessonId) + "\nФайлы: " + sources);
    layout->addWidget(taskLabel);

    if (sources.isEmpty()) {
        QPushButton *addAnswerButton = new QPushButton("Добавить задание");
        layout->addWidget(addAnswerButton);

        connect(addAnswerButton, &QPushButton::clicked, [this, answerId, userId]() {
            QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл");
            if (!filePath.isEmpty()) {
                QString fileName = QFileInfo(filePath).fileName();
                QString destinationDir = QString("Files/answers/%1").arg(userId);
                QDir().mkpath(destinationDir);
                QString destinationPath = destinationDir + "/" + fileName;

                if (QFile::copy(filePath, destinationPath)) {
                    QSqlQuery query;
                    query.prepare("UPDATE answers SET sources = :sources WHERE answer_id = :answer_id AND user_id = :user_id");
                    query.bindValue(":sources", fileName);
                    query.bindValue(":answer_id", answerId);
                    query.bindValue(":user_id", userId);
                    if (query.exec()) {
                        qDebug() << "Файл сохранен и добавлен в базу данных.";
                        refreshUI();
                    } else {
                        qDebug() << "Ошибка обновления базы данных.";
                    }
                } else {
                    QMessageBox::critical(this, "Ошибка", "Не удалось скопировать файл.");
                }
            }
        });
    } else {
        QPushButton *addMoreButton = new QPushButton("Добавить ещё");
        QPushButton *deleteAnswerButton = new QPushButton("Удалить задание");
        layout->addWidget(addMoreButton);
        layout->addWidget(deleteAnswerButton);

        connect(addMoreButton, &QPushButton::clicked, [this, answerId, userId]() {
            QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл");
            if (!filePath.isEmpty()) {
                QString fileName = QFileInfo(filePath).fileName();
                QString destinationDir = QString("Files/answers/%1").arg(userId);
                QDir().mkpath(destinationDir);
                QString destinationPath = destinationDir + "/" + fileName;

                if (QFile::copy(filePath, destinationPath)) {
                    QSqlQuery query;
                    query.prepare("SELECT sources FROM answers WHERE answer_id = :answer_id AND user_id = :user_id");
                    query.bindValue(":answer_id", answerId);
                    query.bindValue(":user_id", userId);
                    if (query.exec() && query.next()) {
                        QString oldSources = query.value("sources").toString();
                        QString newSources = oldSources + ", " + fileName;

                        QSqlQuery updateQuery;
                        updateQuery.prepare("UPDATE answers SET sources = :sources WHERE answer_id = :answer_id AND user_id = :user_id");
                        updateQuery.bindValue(":sources", newSources);
                        updateQuery.bindValue(":answer_id", answerId);
                        updateQuery.bindValue(":user_id", userId);
                        if (updateQuery.exec()) {
                            qDebug() << "Файл добавлен в базу данных.";
                            refreshUI();
                        } else {
                            qDebug() << "Ошибка обновления базы данных.";
                        }
                    }
                } else {
                    QMessageBox::critical(this, "Ошибка", "Не удалось скопировать файл.");
                }
            }
        });

        connect(deleteAnswerButton, &QPushButton::clicked, [this, answerId, userId]() {
            QSqlQuery query;
            query.prepare("DELETE FROM answers WHERE answer_id = :answer_id AND user_id = :user_id");
            query.bindValue(":answer_id", answerId);
            query.bindValue(":user_id", userId);
            if (query.exec()) {
                qDebug() << "Ответ удален из базы данных.";
                refreshUI();
            } else {
                qDebug() << "Ошибка удаления ответа из базы данных.";
            }
        });
    }

    setLayout(layout);
}

void Answer::displayTaskData(int lessonId) {
    QLabel *taskLabel = new QLabel("Задание для lesson_id: " + QString::number(lessonId));
    layout->addWidget(taskLabel);
}

int Answer::getUserId() {
    QFile cashFile("cash.txt");
    if (!cashFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка: не удалось открыть файл cash.txt";
        return -1;  // Возвращаем -1, если файл не открыт
    }

    QTextStream in(&cashFile);
    QString userIdStr = in.readLine();  // Считываем первую строку из файла
    cashFile.close();

    bool ok;
    int userId = userIdStr.toInt(&ok);
    if (!ok) {
        qDebug() << "Ошибка: некорректный ID пользователя в файле cash.txt";
        return -1;  // Возвращаем -1, если ID не удалось преобразовать в целое число
    }

    return userId;
}

QString Answer::getUserType() {
    int userId = getUserId();  // Получаем ID пользователя из cash.txt

    if (userId == -1) {
        qDebug() << "Ошибка: не удалось получить ID пользователя";
        return "";  // Возвращаем пустую строку в случае ошибки
    }

    // Создаем запрос к базе данных для получения роли пользователя
    QSqlQuery query;
    query.prepare("SELECT role FROM users WHERE id = :user_id");
    query.bindValue(":user_id", userId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Ошибка: не удалось найти пользователя с ID " << userId;
        return "";  // Возвращаем пустую строку в случае ошибки
    }

    // Извлекаем роль пользователя из результата запроса
    QString role = query.value("role").toString();
    return role;
}

void Answer::refreshUI() {
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    loadAnswerData();
}

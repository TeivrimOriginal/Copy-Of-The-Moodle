QT       += core gui sql widgets concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    answer.cpp \
    course.cpp \
    courselist.cpp \
    database.cpp \
    lesson.cpp \
    main.cpp \
    mainwindow.cpp \
    page1.cpp \
    page2.cpp \
    page3.cpp \
    pagelogin.cpp \
    pageusertype.cpp \
    temporarydata.cpp

HEADERS += \
    answer.h \
    course.h \
    courselist.h \
    database.h \
    lesson.h \
    mainwindow.h \
    page1.h \
    page2.h \
    page3.h \
    pagelogin.h \
    pageusertype.h \
    temporarydata.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

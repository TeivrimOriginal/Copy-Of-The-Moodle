#ifndef THEMAINPAGE_H
#define THEMAINPAGE_H
#include <QWidget>
#include <QStackedWidget>
class THEMAINPAGE : public QWidget {
    Q_OBJECT
public:
    THEMAINPAGE(QStackedWidget *stack);
private:
    QStackedWidget *stack;
};

#endif // THEMAINPAGE_H

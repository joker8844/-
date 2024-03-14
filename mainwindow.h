#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include "common.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init(QString user);

signals:
    //切换用户
    void sigChangeUser();
    //重新登录
    void sigLoginAgain();

private slots:
    void onGotoTransform(TransformStatus status);

private:
    Ui::MainWindow *ui;

    void windowSignals();
    void buttonsSignals();
};

#endif // MAINWINDOW_H

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "common.h"
#include <QDialog>
#include "mainwindow.h"


namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void on_notReg_clicked();
    void showSetPage();
    void closeButton();

    void on_login_clicked();

    void on_btn_reg_clicked();

    void on_set_ok_clicked();

private:
    Ui::LoginDialog *ui;
    MainWindow *m_mainwindow;
    Common *m_common;
    QNetworkAccessManager *m_manager;
    void readConf();
    void serverSet();
    void saveLoginInfoData(QString username, QString token, QString ip, QString port);
};

#endif // LOGINDIALOG_H


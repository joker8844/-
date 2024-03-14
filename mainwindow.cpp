#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sharewidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //去掉标题栏
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    ui->button_group->setParent(this);

    //处理信号
    //处理窗口信号
    windowSignals();

    //处理按钮信号
    buttonsSignals();

    //stackedWidget窗口切换
    /*
    connect(ui->myFile_page, &MyFileWidget::gotoTransform, this, [=](TransformStatus status){
        //
        ui->button_group->onMapperButtonsClicked("传输列表");
        ui->transform_page->showPage(status);

    });
    */

    // 信号传递
    //connect(ui->share_page, &ShareWidget::gotoTransform, ui->myFile_page, &MyFileWidget::gotoTransform);

    connect(ui->myFile_page, &MyFileWidget::gotoTransform, this, &MainWindow::onGotoTransform);

    connect(ui->share_page, &ShareWidget::gotoTransform, this, &MainWindow::onGotoTransform);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onGotoTransform(TransformStatus status)
{
    ui->button_group->onMapperButtonsClicked("传输列表");
    ui->transform_page->showPage(status);
}

void MainWindow::windowSignals()
{
    //处理信号
    //最小化
    connect(ui->button_group, &ButtonGroup::minWindow, this, &MainWindow::showMinimized);

    //最大化
    connect(ui->button_group, &ButtonGroup::maxWindow, this, [=](){
        static bool mainFlag = false;
        if (!mainFlag) {
            this->showMaximized();
            mainFlag = true;
        } else {
            this->showNormal();
            mainFlag = false;
        }
    });

    //关闭
    connect(ui->button_group, &ButtonGroup::closeWindow, this, &MainWindow::close);

}

void MainWindow::buttonsSignals()
{
    //ui->myFile_page->setStyleSheet("background-color: rgb(29, 212, 200);");
    //ui->share_page->setStyleSheet("background-color: rgb(0, 212, 5);");
    //ui->download_page->setStyleSheet("background-color: rgb(29, 0, 5);");
    //ui->transform_page->setStyleSheet("background-color: rgb(29, 100, 100);");

    connect(ui->button_group, &ButtonGroup::sigMyFile, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->myFile_page);
        //刷新用户文件列表
        ui->myFile_page->refreshFiles();
    });
    connect(ui->button_group, &ButtonGroup::sigShare, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->share_page);
        //刷新共享文件列表
        ui->share_page->refreshFiles();
    });
    connect(ui->button_group, &ButtonGroup::sigDownload, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->download_page);
        //刷新下载榜列表
        ui->download_page->refreshFiles();
    });
    connect(ui->button_group, &ButtonGroup::sigTransform, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->transform_page);
    });

    //切换用户
    connect(ui->button_group, &ButtonGroup::sigSwitchUser, this, [=](){
        emit sigChangeUser();
        //用户切换
        ui->myFile_page->changeUser();
    });
    connect(ui->myFile_page, &MyFileWidget::sigLoginAgain, this, [=](){
        emit sigLoginAgain();
    });

    //默认设置myFile_page
    //ui->stackedWidget->setCurrentWidget(ui->myFile_page);
}

void MainWindow::init(QString user) {
    //设置用户名称
    ui->button_group->setUser(user);
    //刷新用户文件列表
    ui->myFile_page->refreshFiles();
    //默认设置myFile_page
    ui->stackedWidget->setCurrentWidget(ui->myFile_page);
}

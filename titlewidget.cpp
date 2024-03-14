#include "titlewidget.h"
#include "ui_titlewidget.h"
#include <QMouseEvent>

TitleWidget::TitleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleWidget)
{
    ui->setupUi(this);

    ui->logo->setPixmap(QPixmap(":/images/logo.ico").scaled(30,30));

    m_parent = parent;
}

TitleWidget::~TitleWidget()
{
    delete ui;
}

void TitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    //只允许鼠标左键拖动
    if (event->buttons() & Qt::LeftButton) {
        //窗口需要跟着鼠标移动
        //窗口左上角点 = 鼠标当前位置 - 差值
        m_parent->move(event->globalPos() - m_pt); //m_parent loginDialog移动
    }
}

void TitleWidget::mousePressEvent(QMouseEvent *event) {
    //只允许鼠标左键按下
    if (event->buttons() == Qt::LeftButton) {
        //求差值 = 鼠标当前位置 - 窗口左上角位置
        m_pt = event->globalPos() - m_parent->geometry().topLeft(); //m_parent loginDialog左上角

    }
}

void TitleWidget::on_close_clicked()
{
    //m_parent->close();
    emit closeButton();
}

void TitleWidget::on_min_clicked()
{
    m_parent->setWindowState(Qt::WindowMinimized);
}

void TitleWidget::on_set_clicked()
{
    //发送信号
    emit showSetPage();
}

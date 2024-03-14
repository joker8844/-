#include "buttongroup.h"
#include "ui_buttongroup.h"
#include <QDebug>
#include <QPainter>
#include <QSignalMapper>

ButtonGroup::ButtonGroup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ButtonGroup)
{
    ui->setupUi(this);

    //qDebug() << "parent:" << parent->objectName();

    //最小化
    QObject::connect(ui->btnMin, &QToolButton::clicked, this, [=](){
        emit minWindow();
    });

    //最大化
    QObject::connect(ui->btnMax, &QToolButton::clicked, this, [=](){
        static bool flag = false;
        if (!flag) {
            ui->btnMax->setIcon(QIcon(":/images/main/window/title_normal.png"));
        } else {
            ui->btnMax->setIcon(QIcon(":/images/main/window/title_max.png"));
        }
        flag = !flag;
        emit maxWindow();
    });

    //关闭
    QObject::connect(ui->btnClose, &QToolButton::clicked, this, [=](){
        emit closeWindow();
    });

    m_curBtn = ui->btnMyFile;
    defaultPage();

    //将一组由标识的发送者的signal连接在一起，通过setMapping来实现的
    //
    m_mapper = new QSignalMapper(this);

    m_map.insert(ui->btnMyFile->text(), ui->btnMyFile);
    m_map.insert(ui->btnShare->text(), ui->btnShare);
    m_map.insert(ui->btnDownload->text(), ui->btnDownload);
    m_map.insert(ui->btnTransform->text(), ui->btnTransform);

    QMap<QString, QToolButton*>::iterator iter = m_map.begin();
    for (; iter != m_map.end(); iter++) {
        m_mapper->setMapping(iter.value(), iter.key());
        connect(iter.value(), SIGNAL(clicked(bool)), m_mapper, SLOT(map()));
    }

    connect(m_mapper, SIGNAL(mapped(QString)), this , SLOT(onMapperButtonsClicked(QString)));


/*
    connect(ui->btnMyFile, &QToolButton::clicked, this, [=](){
        emit sigMyFile();

        ui->btnMyFile->setStyleSheet("color:white");
        ui->btnShare->setStyleSheet("color:black");
        ui->btnDownload->setStyleSheet("color:black");
        ui->btnTransform->setStyleSheet("color:black");
        ui->btnSwitchUser->setStyleSheet("color:black");
    });
    connect(ui->btnShare, &QToolButton::clicked, this, [=](){
        emit sigShare();

        ui->btnMyFile->setStyleSheet("color:black");
        ui->btnShare->setStyleSheet("color:white");
        ui->btnDownload->setStyleSheet("color:black");
        ui->btnTransform->setStyleSheet("color:black");
        ui->btnSwitchUser->setStyleSheet("color:black");
    });
    connect(ui->btnDownload, &QToolButton::clicked, this, [=](){
        emit sigDownload();

        ui->btnMyFile->setStyleSheet("color:black");
        ui->btnShare->setStyleSheet("color:black");
        ui->btnDownload->setStyleSheet("color:white");
        ui->btnTransform->setStyleSheet("color:black");
        ui->btnSwitchUser->setStyleSheet("color:black");
    });
    connect(ui->btnTransform, &QToolButton::clicked, this, [=](){
        emit sigTransform();

        ui->btnMyFile->setStyleSheet("color:black");
        ui->btnShare->setStyleSheet("color:black");
        ui->btnDownload->setStyleSheet("color:black");
        ui->btnTransform->setStyleSheet("color:white");
        ui->btnSwitchUser->setStyleSheet("color:black");
    });
*/

    connect(ui->btnSwitchUser, &QToolButton::clicked, this, [=](){
        emit sigSwitchUser();

        defaultPage();
    });
}

void ButtonGroup::onMapperButtonsClicked(QString text) { //点击下载榜

    QToolButton* btnTemp = nullptr;  //选中按钮

    QMap<QString, QToolButton*>::iterator iter = m_map.begin();
    for (; iter != m_map.end(); iter++) {
        if (iter.key() == text) {
            btnTemp = (*iter);
        }
    }

    if (btnTemp == m_curBtn) {
        return;
    }

    qDebug() << "m_curBtn1:" << m_curBtn->objectName();
    m_curBtn->setStyleSheet("color:black");
    //1. 当前按钮设置选中颜色
    m_curBtn = btnTemp;
    qDebug() << "m_curBtn2:" << m_curBtn->objectName();
    m_curBtn->setStyleSheet("color:white");

    //2. 发送信号
    if (text == ui->btnMyFile->text()) {
        emit sigMyFile();
    } else if (text == ui->btnShare->text()) {
        emit sigShare();
    } else if (text == ui->btnDownload->text()) {
        emit sigDownload();
    } else if (text == ui->btnTransform->text()) {
        emit sigTransform();
    }
}

void ButtonGroup::defaultPage() {
    //默认是btnMyFile;
    m_curBtn->setStyleSheet("color:black");
    m_curBtn = ui->btnMyFile;
    ui->btnMyFile->setStyleSheet("color:white");
}

ButtonGroup::~ButtonGroup()
{
    delete ui;
}

void ButtonGroup::paintEvent(QPaintEvent *event) {
    QPainter p(this);

    QPixmap bg(":/images/main/bk_top.png");
    p.drawPixmap(0,0,this->width(),this->height(), bg);
}

void ButtonGroup::setUser(QString user) {
    ui->btnUser->setText(user);
}

//方法
//函数
void ButtonGroup::setParent(QWidget *parent) {
    //m_parent是MainWindow
    m_parent = parent;
}

void ButtonGroup::mouseMoveEvent(QMouseEvent *event)
{
    //只允许鼠标左键拖动
    if (event->buttons() & Qt::LeftButton) {
        //窗口需要跟着鼠标移动
        //窗口左上角点 = 鼠标当前位置 - 差值
        m_parent->move(event->globalPos() - m_pt); //m_parent loginDialog移动
    }
}

void ButtonGroup::mousePressEvent(QMouseEvent *event) {
    //只允许鼠标左键按下
    if (event->buttons() == Qt::LeftButton) {
        //求差值 = 鼠标当前位置 - 窗口左上角位置
        m_pt = event->globalPos() - m_parent->geometry().topLeft(); //m_parent loginDialog左上角
    }
}

#include "downloadwidget.h"
#include "ui_downloadwidget.h"
#include <QPainter>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "networkdata.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QHeaderView>

DownloadWidget::DownloadWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadWidget)
{
    ui->setupUi(this);

    m_common = Common::getInstance();
    m_manager = m_common->getNetworkAccessManager();

    initTableWidget();
}

DownloadWidget::~DownloadWidget()
{
    delete ui;
}

void DownloadWidget::paintEvent(QPaintEvent *event) {
    QPainter p(this);

    QPixmap bg(":/images/main/bk_page.png");
    p.drawPixmap(0,0,this->width(),this->height(), bg);
}

void DownloadWidget::initTableWidget()
{
    //表头相关设置
    //设置列数
    ui->tableWidget->setColumnCount(3);
    //设置列的宽度
    ui->tableWidget->horizontalHeader()->setDefaultSectionSize(300);

    //设置表头内容
    QStringList header;
    header.append("排名");
    header.append("文件名");
    header.append("下载量");
    ui->tableWidget->setHorizontalHeaderLabels(header);

    //设置字体
    //获取表头原来的字体
    QFont font = ui->tableWidget->horizontalHeader()->font();
    font.setBold(true);
    ui->tableWidget->horizontalHeader()->setFont(font);

    //设置垂直方向高度
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(40);


    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //单行选择
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑

    //设置表头不可编辑
    ui->tableWidget->horizontalHeader()->setSectionsClickable(false);
    //设置垂直头不可见，不自动显示行号
    ui->tableWidget->verticalHeader()->setVisible(false);

    //设置表头样式
    ui->tableWidget->horizontalHeader()->setStyleSheet(
                "QHeaderView::section{"
                "background: skyblue;"
                "font: 12pt \"黑体\";"
                "height: 35px;"
                "border: 1px solid #FF00FF;"
                "}"
    );

    //设置第0行100
    ui->tableWidget->setColumnWidth(0, 100);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}


void DownloadWidget::refreshFiles()
{
    getShareFilesCount();
}

//获取共享文件数量
void DownloadWidget::getShareFilesCount()
{
    m_shareFilesCount = 0;

    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/sharefiles?cmd=count
    QString url = QString("http://%1:%2/sharefiles?cmd=count").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QNetworkReply *reply = m_manager->get(request);

    //读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        //读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        m_shareFilesCount = data.toInt();
        //m_shareFilesCount = NetworkData::getCount(data);
        if (m_shareFilesCount > 0) {

            //将数据写到 tableWidget
            clearShareFileDownload();

            //请求下载榜
            getFileDownload();
        } else {
            //共享文件数量等于0
            clearShareFileDownload();

            clearTableWidget();
        }

        //立即销毁
        reply->deleteLater();

    });

}

//获取文件下载榜
void DownloadWidget::getFileDownload()
{
qDebug() << "-----------------------------" ;
    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/sharefiles?cmd=pvdesc
    QString url = QString("http://%1:%2/sharefiles?cmd=pvdesc").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

/*
{
    "count": 1,
    "start": 0,
}
*/
    QJsonObject paramsObj;
    paramsObj.insert("start", 0);
    paramsObj.insert("count", m_shareFilesCount);
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);


    //读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        //读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        m_fileDownload = NetworkData::getFileDownload(data);
        qDebug() << "m_fileDownload size()" << m_fileDownload.size();

        //清空m_fileDownload
        clearShareFileDownload();

        //立即销毁
        reply->deleteLater();

    });

}

void DownloadWidget::clearShareFileDownload()
{
    //int size = m_fileDownload.size();
    //for (int i=0;i<size;i++) {
    while (m_fileDownload.size()>0) {
        FileDownload *temp = m_fileDownload.takeFirst();
        delete temp;
    }
}

void DownloadWidget::clearTableWidget()
{
    int rowCount = ui->tableWidget->rowCount(); //获取表单行数
    for (int i=0; i<rowCount; ++i) {
        ui->tableWidget->removeRow(0); //删除之前的item
    }
}

void DownloadWidget::refreshTableWidget()
{
    clearTableWidget();

    //元素个数
    int size = m_fileDownload.size();
    int rowCount = 0;
    for (int i=0;i<size;i++) {
        FileDownload *temp = m_fileDownload.at(i);
        if (temp != NULL) {
            ui->tableWidget->insertRow(rowCount); //插入新的一行
            QTableWidgetItem *item1 = new QTableWidgetItem;
            QTableWidgetItem *item2 = new QTableWidgetItem;
            QTableWidgetItem *item3 = new QTableWidgetItem;

            item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item2->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item3->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            QFont font;
            font.setPointSize(15); //设置字体大小
            item1->setFont(font);
            item1->setText(QString("%1").arg(i+1));

            item2->setText(temp->fileName);
            item3->setText(QString("%1").arg(temp->pv));

            ui->tableWidget->setItem(rowCount, 0, item1);
            ui->tableWidget->setItem(rowCount, 1, item2);
            ui->tableWidget->setItem(rowCount, 2, item3);

            rowCount++;
        }
    }

}


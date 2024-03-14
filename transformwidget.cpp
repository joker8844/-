#include "transformwidget.h"
#include "ui_transformwidget.h"
#include <QPainter>
#include "uploadlayout.h"
#include "downloadlayout.h"
#include "logininfoinstance.h"
#include <QFile>

TransformWidget::TransformWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransformWidget)
{
    ui->setupUi(this);


    //垂直布局 (进度条UI)，
    //布局单独的做成一个类

    //设置上传布局类
    UploadLayout *uploadLayout = UploadLayout::getInstance();
    uploadLayout->setUploadLayout(ui->upload_scroll);

    //设置下载布局类
    DownloadLayout *downloadLayout = DownloadLayout::getInstance();
    downloadLayout->setDownloadLayout(ui->download_scroll);

    ui->tabWidget->setCurrentWidget(ui->tabUpload);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [=](int index) {
        if (index == 0) {   //上传列表

        } else if (index == 1) {   //下载列表

        } else if (index == 2) {   //传输记录
            //显示数据传输记录
            dispayDataRecord();
        }
    });
}

TransformWidget::~TransformWidget()
{
    delete ui;
}

void TransformWidget::showPage(TransformStatus status)
{
    if (status == TransformStatus::Upload) {
        ui->tabWidget->setCurrentWidget(ui->tabUpload);
    } else if (status == TransformStatus::Download) {
        ui->tabWidget->setCurrentWidget(ui->tabDownload);
    }
}


//传输数据记录在UI显示
void TransformWidget::dispayDataRecord(QString path)
{
    // conf/record/milo.txt
    QString userFilePath = QString("%1/%2.txt").arg(path).arg(LoginInfoInstance::getInstance()->user());
    qDebug() << "userFilePath:" << userFilePath;

    QFile file(userFilePath);
    if (!file.open(QIODevice::ReadOnly)) { //只读方式打开
        qDebug() << "file.open(QIODevice::ReadOnly) err";
        return;
    }


    QByteArray array = file.readAll();

    //toLocal8Bit转为本地字符集
    //fromLocal8Bit本地字符集转为utf-8
    ui->record_mg->setText(QString::fromLocal8Bit(array));

    if (file.isOpen()) {
        file.close();
    }
}

void TransformWidget::on_clear_clicked()
{
    ui->record_mg->setText("");
}

void TransformWidget::paintEvent(QPaintEvent *event) {
    QPainter p(this);

    QPixmap bg(":/images/main/bk_page.png");
    p.drawPixmap(0,0,this->width(),this->height(), bg);
}

#include "filepropertyinfodialog.h"
#include "ui_filepropertyinfodialog.h"

FilePropertyInfoDialog::FilePropertyInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilePropertyInfoDialog)
{
    ui->setupUi(this);

    QString style1 = QString("color: rgb(11, 11, 11);font: 75 14pt \"Agency FB\";");
    QString style2 = QString("font: 14pt \"隶书\";color: rgb(255, 156, 16);");

    ui->lbl_fileName->setStyleSheet(style1);
    ui->lbl_fileSize->setStyleSheet(style1);
    ui->lbl_download->setStyleSheet(style1);
    ui->lbl_shareStatus->setStyleSheet(style1);
    ui->lbl_uploadTime->setStyleSheet(style1);
    ui->lbl_uploadUser->setStyleSheet(style1);
    ui->lbl_pv->setStyleSheet(style1);

    ui->val_fileName->setStyleSheet(style2);
    ui->val_fileSize->setStyleSheet(style2);
    ui->val_download->setStyleSheet(style2);
    ui->val_shareStatus->setStyleSheet(style2);
    ui->val_uploadTime->setStyleSheet(style2);
    ui->val_uploadUser->setStyleSheet(style2);
    ui->val_pv->setStyleSheet(style2);

    ui->val_download->setOpenExternalLinks(true); //label内容作为超链接内容
}

FilePropertyInfoDialog::~FilePropertyInfoDialog()
{
    delete ui;
}

void FilePropertyInfoDialog::setFileInfo(FileInfo *fileInfo)
{
    ui->val_fileName->setText(fileInfo->fileName);
    int size = fileInfo->size;

    //1kb = 1024个字节
    //展示给用户看的是KB,MB
    if (size>=1024 && size < 1024*1024) {
        ui->val_fileSize->setText(QString("%1 KB").arg(size/1024.0));
    }
        //else if (size>=1024*1024 && size < 1024*1024*1024) {
    else {
        ui->val_fileSize->setText(QString("%1 MB").arg(size/1024.0/1024.0));
    }

    if (fileInfo->shareStatus == 0) {
        ui->val_shareStatus->setText("没有分享");
    } else if (fileInfo->shareStatus == 1) {
        ui->val_shareStatus->setText("已经分享");
    }


    ui->val_uploadTime->setText(fileInfo->createTime);
    ui->val_uploadUser->setText(fileInfo->user);
    ui->val_pv->setText(QString("被下载 %1 次").arg(fileInfo->pv));

    //传一个链接
    //<a href="http://192.168.139.131:80/group1/M00/00/00/wKiLg1-jbP-AfQYXAABf_SnU0l4565.png">图片1</a>
    QString tmp = QString("<a href=\"%1\">%2</a>").arg(fileInfo->url).arg(fileInfo->url);
    ui->val_download->setText(tmp);
}

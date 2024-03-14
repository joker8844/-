#include "filedataprogress.h"
#include "ui_filedataprogress.h"

FileDataProgress::FileDataProgress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileDataProgress)
{
    ui->setupUi(this);
}

FileDataProgress::~FileDataProgress()
{
    delete ui;
}


//设置文件名称
void FileDataProgress::setFileName(QString fileName)
{
    ui->label->setText(fileName);
    ui->progressBar->setValue(0);
    ui->progressBar->setMinimum(0); //最小值
}

void FileDataProgress::setProgress(int value, int max)
{
    ui->progressBar->setValue(value);
    ui->progressBar->setMaximum(max);
}

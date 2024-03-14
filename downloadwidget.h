#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QWidget>
#include "common.h"
#include "fileinfo.h"

namespace Ui {
class DownloadWidget;
}

class DownloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadWidget(QWidget *parent = 0);
    ~DownloadWidget();
    void refreshFiles();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::DownloadWidget *ui;
    Common *m_common;
    QNetworkAccessManager *m_manager;

    int m_shareFilesCount;

    QList<FileDownload*> m_fileDownload; //下载榜

    void initTableWidget();



    //获取共享文件数量
    void getShareFilesCount();

    void getFileDownload();

    void refreshTableWidget();

    void clearShareFileDownload();

    void clearTableWidget();
};

#endif // DOWNLOADWIDGET_H

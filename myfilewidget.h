#ifndef MYFILEWIDGET_H
#define MYFILEWIDGET_H

#include <QWidget>
#include "mymenu.h"
#include "common.h"
#include "logininfoinstance.h"
#include "networkdata.h"
#include "uploadtask.h"
#include "downloadtask.h"
#include <QTimer>

namespace Ui {
class MyFileWidget;
}

class MyFileWidget : public QWidget
{
    Q_OBJECT

public:
    enum MyFileDisplay { Normal, Asc, Desc};

    explicit MyFileWidget(QWidget *parent = 0);
    ~MyFileWidget();
    //刷新用户文件列表
    void refreshFiles();

    //切换用户
    void changeUser();

private:
    Ui::MyFileWidget *ui;

    Common *m_common;
    UploadTask *m_uploadTask;
    DownloadTask *m_downloadTask;
    LoginInfoInstance *m_loginInfo;
    QNetworkAccessManager *m_manager;


    int m_myFilesCount;
    QList<FileInfo*> m_fileList; //任务列表
    QStringList m_fileTypeList;

    QTimer m_uploadFileTimer;
    QTimer m_downloadFileTimer;


    MyMenu *m_menuItem;
    MyMenu *m_menuEmpty;

    QAction *m_actionDownload;
    QAction *m_actionShare;
    QAction *m_actionDelete;
    QAction *m_actionProperty;
    QAction *m_actionDownloadAsc;
    QAction *m_actionDownloadDesc;
    QAction *m_actionRefresh;
    QAction *m_actionUpload;

    void initListWidget();
    void addMenu();
    void menuActions();
    void getMyFileCount(MyFileDisplay cmd=Normal);
    void getMyFileList(MyFileDisplay cmd=Normal);
    void showFileItems();
    void addListWidgetItem(FileInfo *fileInfo);
    void addUploadItem();
    void clearItems();
    void clearFileList();
    void clearAllTask();
    void refreshFileItems();

    //分享文件
    void dealfile(QString cmd);
    void shareFile(FileInfo *fileInfo);
    void deleteFile(FileInfo *fileInfo);
    void showFileProperty(FileInfo *fileInfo);
    void uploadFile();
    void uploadFile(UploadFileInfo *uploadFileInfo);
    void addUploadFiles();
    void addDownloadFiles();
    void getMd5Str();

    //定时器
    void checkTaskList();
    void uploadFilesAction();
    void downloadFilesAction();

private slots:
    void onRightMenu(const QPoint &pos);
signals:
    //重新登录
    void sigLoginAgain();
    void gotoTransform(TransformStatus status);

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // MYFILEWIDGET_H

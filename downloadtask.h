#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H


#include <QString>
#include <QList>
#include "filedataprogress.h"
#include "fileinfo.h"
#include <QFile>

struct DownloadFileInfo
{
    QFile *file;            //文件
    QString user;           //下载用户
    QString md5;            //文件的md5值
    QString fileName;       //文件名称
    QString filePath;       //文件路径
    QString url;            //下载地址
    bool isShareTask;       //是否是共享文件任务, true是共享文件任务,false不是共享文件任务
    FileDataProgress *fdp;  //进度条
};

//上传任务列表类, 单例模式
class DownloadTask
{
public :
    //获取UploadTask唯一实例对象
    static DownloadTask* getInstance();

    //判断任务列表是否为空
    bool isEmpty();

    //取出任务
    DownloadFileInfo* takeTask();

    //添加文件到上传任务列表中
    int appendDownloadTask(FileInfo *fileInfo, QString filePath, bool isShareTask = false);

    //删除任务
    void delDownloadTask();

    void clearList();
private:
    DownloadTask();
    ~DownloadTask();

    //静态数据成员，类中声明，类外必须定义
    static DownloadTask* m_instance;

private:
    QList<DownloadFileInfo*> m_fileList; //下载的文件

};

#endif // DOWNLOADTASK_H

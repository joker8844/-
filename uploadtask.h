#ifndef UPLOADTASK_H
#define UPLOADTASK_H

#include <QString>
#include <QList>
#include "filedataprogress.h"

#define UPLOAD_NOT          0   //未上传
#define UPLOADING           1   //正在上传
#define UPLOAD_FINISHED     2   //上传完成
#define UPLOAD_FAILD        3   //上传失败
#define UPLOAF_FILE_EXISTE  4   //上传的文件已存在

struct UploadFileInfo
{
    QString md5;            //文件的md5值
    QString fileName;       //文件名称
    QString filePath;       //文件路径
    qint64 size;            //文件大小
    FileDataProgress *fdp;  //进度条
    int uploadStatus;       //0.未上传，1.正在上传，2.上传完成
};


//上传任务列表类, 单例模式
class UploadTask
{
public :
    //获取UploadTask唯一实例对象
    static UploadTask* getInstance();

    //添加文件到上传任务列表中
    int appendUploadTask(QString filePath);

    //判断任务列表是否为空
    bool isEmpty();

    //取出任务
    UploadFileInfo* takeTask();

    //删除任务
    void delUploadTask();

    //清空列表
    void clearList();

private:
    UploadTask();
    ~UploadTask();

    //静态数据成员，类中声明，类外必须定义
    static UploadTask* m_instance;

private:
    QList<UploadFileInfo*> m_fileList;

};

#endif // UPLOADTASK_H

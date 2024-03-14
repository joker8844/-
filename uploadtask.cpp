#include "uploadtask.h"
#include "common.h"
#include <QFileInfo>
#include <QVBoxLayout>
#include "uploadlayout.h"

UploadTask* UploadTask::m_instance = new UploadTask;

UploadTask::UploadTask()
{

}

UploadTask* UploadTask::getInstance() {
    return m_instance;
}

// -1 文件大于30M
int UploadTask::appendUploadTask(QString filePath)
{
    //filePath 文件的路径
    //user=\"%1\" filename=\"%2\" md5=\"%3\" size=%4

    qint64 fileSize = QFileInfo(filePath).size();
    //判断文件的大小， 最大30M
    if (fileSize > 30 * 1024 * 1024) {
        qDebug() << "文件太大，最大允许上传30M";
        return -1;
    }

    UploadFileInfo *uploadFile = new UploadFileInfo;

    //截取字符串
    int pos = filePath.lastIndexOf("/", -1) + 1;
    uploadFile->fileName = filePath.mid(pos);
    uploadFile->filePath = filePath;
    uploadFile->size = fileSize;
    uploadFile->md5 = Common::getInstance()->getFileMd5(filePath);
    uploadFile->uploadStatus = UPLOAD_NOT;

    //上传进度条（显示）, 将进度条UI显示在vLayout上
    FileDataProgress *fdp = new FileDataProgress();
    fdp->setFileName(uploadFile->fileName);
    uploadFile->fdp = fdp;

    QVBoxLayout *vLayout = UploadLayout::getInstance()->getUploadLayout();
    //每一个都添加最上面
    vLayout->insertWidget(0, fdp);

    //m_fileList保存上传文件的列表
    m_fileList.append(uploadFile);

    return 0;
}

bool UploadTask::isEmpty() {
    return m_fileList.isEmpty();
}

UploadFileInfo* UploadTask::takeTask() {

    UploadFileInfo *temp = NULL;
    if (m_fileList.size() > 0) {
        temp = m_fileList.at(0);
        temp->uploadStatus = UPLOADING;
    }

    return temp;
}

void UploadTask::delUploadTask() {

    //上传完成后需要处理的

    //1. FileDataProgress（fdp）对象从vLayout中移除
    //2. 删除FileDataProgress（fdp）对象
    //3. 删除UploadFileInfo对象

    for (int i=0; i<m_fileList.size(); i++) {
        UploadFileInfo *tmp = m_fileList.at(i);
        if (tmp->uploadStatus == UPLOAD_FINISHED ||
                tmp->uploadStatus == UPLOAD_FAILD ||
                tmp->uploadStatus == UPLOAF_FILE_EXISTE) {
            UploadFileInfo *uploadFileInfo = m_fileList.takeAt(0);
            if (uploadFileInfo != NULL) {
                //第一步
                QVBoxLayout *vLayout = UploadLayout::getInstance()->getUploadLayout();
                vLayout->removeWidget(uploadFileInfo->fdp);

                //第二步
                delete uploadFileInfo->fdp;

                //第三步
                delete uploadFileInfo;
            }
        }
    }

}

void UploadTask::clearList()
{
    while (m_fileList.size()>0) {
        UploadFileInfo *uploadFileInfo = m_fileList.takeAt(0);
        if (uploadFileInfo != NULL) {
            //第一步
            QVBoxLayout *vLayout = UploadLayout::getInstance()->getUploadLayout();
            vLayout->removeWidget(uploadFileInfo->fdp);

            //第二步
            delete uploadFileInfo->fdp;

            //第三步
            delete uploadFileInfo;
        }
    }
}

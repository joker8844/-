#include "myfilewidget.h"
#include "ui_myfilewidget.h"
#include "mymenu.h"
#include "common.h"
#include <QListView>
#include <QDebug>
#include <QAction>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDir>
#include <QPainter>
#include "filepropertyinfodialog.h"
#include <QFileDialog>

MyFileWidget::MyFileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyFileWidget)
{
    ui->setupUi(this);

    initListWidget();

    addMenu();

    m_manager = Common::getInstance()->getNetworkAccessManager();
    m_uploadTask = UploadTask::getInstance();
    m_downloadTask = DownloadTask::getInstance();

    checkTaskList();
}

MyFileWidget::~MyFileWidget()
{
    delete ui;
}

void MyFileWidget::paintEvent(QPaintEvent *event) {
    QPainter p(this);

    QPixmap bg(":/images/main/bk_page.png");
    p.drawPixmap(0,0,this->width(),this->height(), bg);
}

void MyFileWidget::initListWidget()
{
    m_common = Common::getInstance();
    m_loginInfo = LoginInfoInstance::getInstance();

    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setIconSize(QSize(80,80));
    ui->listWidget->setGridSize(QSize(100, 120));

    //设置QListView大小改变时，图标的调整模式, 默认时固定的，可以改成自动调整
    ui->listWidget->setResizeMode(QListView::Adjust); //自适应布局
    //设置图标拖动状态，QListView::Static表示不可拖动
    ui->listWidget->setMovement(QListView::Static);

    //listWidget右键菜单
    //发出QWidget::customContextMenuRequested() 信号
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &MyFileWidget::onRightMenu);
    connect(ui->listWidget, &QListWidget::itemPressed, this, [=](QListWidgetItem *item){
        QString text = item->text();
        if (text == "上传文件") {
            //uploadFile();
            //添加文件到上传任务列表
            addUploadFiles();
        }
    });
}

void MyFileWidget::checkTaskList()
{

    connect(&m_uploadFileTimer, &QTimer::timeout, this, [=](){
        //定时执行
        //上传文件处理，取出上传任务列表的首任务，上传完后，再取下一个任务
        uploadFilesAction();
    });

    //启动定时器
    m_uploadFileTimer.start(500);


    connect(&m_downloadFileTimer, &QTimer::timeout, this, [=](){
        //定时执行
        //上传文件处理，取出上传任务列表的首任务，上传完后，再取下一个任务
        downloadFilesAction();
    });

    //启动定时器
    m_downloadFileTimer.start(500);

}

void MyFileWidget::uploadFilesAction()
{
    //取出上传任务列表的首任务
    if (m_uploadTask->isEmpty()) {
        //qDebug() << "任务列表为空";
        return;
    }

    UploadFileInfo *uploadFileInfo =  m_uploadTask->takeTask();
    if (uploadFileInfo == NULL) {
        qDebug() << "任务列表为空";
        return;
    }


    //上传文件， 秒传文件
    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/md5
    QString url = QString("http://%1:%2/md5").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject paramsObj;
    paramsObj.insert("token", m_loginInfo->token());
    paramsObj.insert("md5", uploadFileInfo->md5);
    paramsObj.insert("filename", uploadFileInfo->fileName);
    paramsObj.insert("user", m_loginInfo->user());
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);

    //读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        //读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

/*
005：上传的文件已存在
006: 秒传成功
007: 秒传失败
111: Token验证失败
*/
        QString code = NetworkData::getCode(data);
        if (code == "005") {  //上传的文件已存在, 秒传成功
            uploadFileInfo->uploadStatus = UPLOAF_FILE_EXISTE;

            //删除已经完成的上传任务
            m_uploadTask->delUploadTask();

            m_common->writeRecord(m_loginInfo->user(),
                                  uploadFileInfo->fileName,
                                  code);
        } else if (code == "006") {  //秒传成功
            m_uploadTask->delUploadTask();

            m_common->writeRecord(m_loginInfo->user(),
                                  uploadFileInfo->fileName,
                                  code);
        } else if (code == "007") {  //秒传失败
            m_common->writeRecord(m_loginInfo->user(),
                                  uploadFileInfo->fileName,
                                  code);

            //服务器并没有此文件，需要真正上传文件
            uploadFile(uploadFileInfo);
        } else if (code == "111") { //token验证失败
            QMessageBox::critical(this, "账号异常", "请重新登录");
            emit sigLoginAgain();
            return;
        }

        //立即销毁
        reply->deleteLater();

    });

}

void MyFileWidget::downloadFilesAction() {
    //取出上传任务列表的首任务
    if (m_downloadTask->isEmpty()) {
        //qDebug() << "任务列表为空";
        return;
    }

    DownloadFileInfo *downloadFileInfo =  m_downloadTask->takeTask();
    if (downloadFileInfo == NULL) {
        qDebug() << "任务列表为空";
        return;
    }

    QFile *file = downloadFileInfo->file;
    //http://192.168.139.131:80/group1/M00/00/00/wKiLg1-X3T-Aa-ODAAABasdNeVk96.json
    QUrl url = QUrl(downloadFileInfo->url);
    QNetworkRequest request;
    request.setUrl(url);

    //去下载文件了
    QNetworkReply *reply = m_manager->get(request); //请求方法
    if (reply == NULL) {
        //删除任务
        m_downloadTask->delDownloadTask();

        qDebug() << "下载文件失败";
        return;
    }

    //读取数据并写入文件
    connect(reply, &QNetworkReply::readyRead, [=]() {
        if (file != NULL) {
            file->write(reply->readAll()); //读取数据后，需要保存到文件
        }
    });

    connect(reply, &QNetworkReply::finished, [=]() {
        reply->deleteLater();

        m_common->writeRecord(LoginInfoInstance::getInstance()->user(),
                              downloadFileInfo->fileName,
                              "090"); //090下载成功

        //等待
        m_common->sleep(3000);

        //删除下载任务
        m_downloadTask->delDownloadTask();
    });

    //显示文件下载进度
    connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesSent, qint64 bytesTotal){
        downloadFileInfo->fdp->setProgress(bytesSent/1024, bytesTotal/1024);
    });

}

//下载 ,
void MyFileWidget::addDownloadFiles()
{
    emit gotoTransform(TransformStatus::Download);

    //当前选中的item
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (item == NULL) {
        qDebug() << "item == NULL";
        return;
    }

    FileInfo *fileInfo = NULL;
    //FileInfo
    int size = m_fileList.size();
    for (int i=0;i<size;i++) {
        fileInfo = m_fileList.at(i);
        if (fileInfo != NULL) {
            if (fileInfo->fileName == item->text()) {
                //找到FileInfo对象了
                //打开保存文件的对话框
                QString filePath = QFileDialog::getSaveFileName(this, "请选择保存文件路径", item->text());
                qDebug () << "filePath:" << filePath;
                if (filePath.isEmpty()) {
                    qDebug() << "filePathName.isEmpty()";
                    return;
                }


                //将需要下载的文件添加到下载任务列表
                int res = m_downloadTask->appendDownloadTask(fileInfo, filePath);
                if (res == -2) {
                    m_common->writeRecord(LoginInfoInstance::getInstance()->user(),
                                          fileInfo->fileName,
                                          "091"); //091 下载失败
                }
            }
        }
    }
}

//上传
void MyFileWidget::addUploadFiles()
{
    //发送信号
    emit gotoTransform(TransformStatus::Upload);
    QStringList filePathlist = QFileDialog::getOpenFileNames();
    for (int i=0; i<filePathlist.size(); i++) {
        QString filePath = filePathlist.at(i);

        //添加到上传任务列表
        int res = m_uploadTask->appendUploadTask(filePath);
        if (res == -1) {
            QMessageBox::warning(this, "上传警告", "文件大小不能超过30M");
        }
    }
}

void MyFileWidget::uploadFile(UploadFileInfo *uploadFileInfo)
{
    //上传文件到服务器

/*
------WebKitFormBoundaryDQAR0QX1ojAyzAre\r\n
Content-Disposition: form-data; name="file"; filename="logo.png"\r\n
Content-Type: image/png\r\n
\r\n
真正的文件内容\r\n
------WebKitFormBoundaryDQAR0QX1ojAyzAre
*/

    QFile file(uploadFileInfo->filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    LoginInfoInstance *login = LoginInfoInstance::getInstance();

    QString boundary = m_common->getBoundary();
    QByteArray data;
    data.append(boundary);
    data.append("\r\n");

    data.append("Content-Disposition: form-data; ");
    data.append(QString("user=\"%1\" filename=\"%2\" md5=\"%3\" size=%4")
                .arg(login->user())
                .arg(uploadFileInfo->fileName)
                .arg(uploadFileInfo->md5)
                .arg(uploadFileInfo->size));
    data.append("\r\n");
    data.append("Content-Type: application/octet-stream");
    data.append("\r\n");
    data.append("\r\n");


    //上传文件
    data.append(file.readAll());
    data.append("\r\n");
    data.append(boundary);

    if (file.isOpen()) {
        file.close();
    }

    QString url = QString("http://%1:%2/upload").arg(login->ip()).arg(login->port());
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    //发送http请求
    QNetworkReply *reply = m_manager->post(request, data);
    if (reply == NULL) {
        qDebug() << "请求失败";
        return;
    }



    //显示文件上传进度
    connect(reply, &QNetworkReply::uploadProgress, this, [=](qint64 bytesSent, qint64 bytesTotal){
        //bytesSent 上传的字节数
        //bytesTotal 文件需要上传的总字节数

        if (bytesTotal != 0) {
            //显示进度条(设置进度条)
            uploadFileInfo->fdp->setProgress(bytesSent/1024, bytesTotal/1024);
        }

    });

    connect(reply, &QNetworkReply::finished, this, [=](){
        //文件上传完成后
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
        } else {
            QByteArray json = reply->readAll();
            qDebug() << "array:" <<QString(json);
/*
008: 上传成功
009: 上传失败
*/
            QString code = NetworkData::getCode(json);
            if (code == "008") {
                qDebug() << "上传成功";
                uploadFileInfo->uploadStatus = UPLOAD_FINISHED;

                m_common->writeRecord(m_loginInfo->user(),
                                      uploadFileInfo->fileName,
                                      code);

                //刷新
                getMyFileCount();

            } else if (code == "009") {
                qDebug() << "上传失败";
                uploadFileInfo->uploadStatus = UPLOAD_FAILD;

                m_common->writeRecord(m_loginInfo->user(),
                                      uploadFileInfo->fileName,
                                      code);
            }

            //获取到上传任务列表
            UploadTask *uploadTask = UploadTask::getInstance();
            //删除任务
            uploadTask->delUploadTask();

        }
        reply->deleteLater();
    });
}

void MyFileWidget::uploadFile()
{
    //选择文件
    QString filePath = QFileDialog::getOpenFileName();
    qDebug() << "filePath:" << filePath;

    //上传文件到服务器

/*
------WebKitFormBoundaryDQAR0QX1ojAyzAre\r\n
Content-Disposition: form-data; name="file"; filename="logo.png"\r\n
Content-Type: image/png\r\n
\r\n
真正的文件内容\r\n
------WebKitFormBoundaryDQAR0QX1ojAyzAre
*/

    QFile file(filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    //截取字符串
    int pos = filePath.lastIndexOf("/", -1) + 1;
    qDebug() << "pos:" << pos;
    QString fileName = filePath.mid(pos);
    qDebug() << "fileName:" << fileName;

    LoginInfoInstance *login = LoginInfoInstance::getInstance();

    QString boundary = m_common->getBoundary();
    QByteArray data;
    data.append(boundary);
    data.append("\r\n");

    data.append("Content-Disposition: form-data; ");
    data.append(QString("user=\"%1\" filename=\"%2\" md5=\"%3\" size=%4")
                .arg(login->user())
                .arg(fileName)
                .arg(m_common->getFileMd5(filePath))
                .arg(file.size()));
    data.append("\r\n");
    data.append("Content-Type: application/octet-stream");
    data.append("\r\n");
    data.append("\r\n");


    //上传文件
    data.append(file.readAll());
    data.append("\r\n");
    data.append(boundary);

    if (file.isOpen()) {
        file.close();
    }

    QString url = QString("http://%1:%2/upload").arg(login->ip()).arg(login->port());
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    //发送http请求
    QNetworkReply *reply = m_manager->post(request, data);
    if (reply == NULL) {
        qDebug() << "请求失败";
        return;
    }

    connect(reply, &QNetworkReply::finished, this, [=](){
        //文件上传完成后
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
        } else {
            QByteArray json = reply->readAll();
            qDebug() << "array:" <<QString(json);
/*
008: 上传成功
009: 上传失败
*/
            QString code = NetworkData::getCode(json);
            if (code == "008") {
                qDebug() << "上传成功";

                /*
                //login_bk.jpg
                int spos = fileName.lastIndexOf(".", -1) + 1;
                qDebug() << "pos:" << spos;
                QString suffixName = fileName.mid(spos);
                qDebug() << "suffixName:" << suffixName;
                QString fileTypeName = QString("%1.png").arg(suffixName);
                qDebug() << "fileTypeName:" << fileTypeName;
                QString imageName = m_common->getFileType(fileTypeName);
                qDebug() << "imageName:" << imageName;
                QString filePath = QString("%1/%2").arg(FILE_TYPE_DIR).arg(imageName);
                qDebug() << "imageName:" << imageName;

                //添加items(图片/文字)到listWidget
                ui->listWidget->addItem(new QListWidgetItem(QIcon(filePath), fileName));
                */

                //刷新
                getMyFileCount();

            } else if (code == "009") {
                qDebug() << "上传失败";
            }

        }
        reply->deleteLater();
    });


}

//显示右键菜单
void MyFileWidget::onRightMenu(const QPoint &pos)
{
    QListWidgetItem *item = ui->listWidget->itemAt(pos);
    if (item == NULL) { //没有点击图标
        qDebug() << "NULL";

        m_menuEmpty->exec(QCursor::pos()); //QCursor::pos() 鼠标当前位置

    } else { //点击了图标
        qDebug() << "clieck";
        if (item->text() == "上传文件") {
            //如果是上传文件，没有右击菜单
            return;
        }

        m_menuItem->exec(QCursor::pos()); //QCursor::pos() 鼠标当前位置
    }
}


void MyFileWidget::addMenu()
{
    //点击item显示的菜单
    m_menuItem = new MyMenu(this);

    m_actionDownload = new QAction("下载", this);
    m_actionShare = new QAction("分享", this);
    m_actionDelete = new QAction("删除", this);
    m_actionProperty = new QAction("属性", this);

    m_menuItem->addAction(m_actionDownload);
    m_menuItem->addAction(m_actionShare);
    m_menuItem->addAction(m_actionDelete);
    m_menuItem->addAction(m_actionProperty);


    //点击空白位置显示的菜单
    m_menuEmpty = new MyMenu(this);

    m_actionDownloadAsc = new QAction("按下载量升序", this);
    m_actionDownloadDesc = new QAction("按下载量降序", this);
    m_actionRefresh = new QAction("刷新", this);
    m_actionUpload = new QAction("上传", this);

    m_menuEmpty->addAction(m_actionDownloadAsc);
    m_menuEmpty->addAction(m_actionDownloadDesc);
    m_menuEmpty->addAction(m_actionRefresh);
    m_menuEmpty->addAction(m_actionUpload);

    menuActions();
}

void MyFileWidget::menuActions() {
    connect(m_actionDownload, &QAction::triggered, this, [=](){
        qDebug() << "下载";
        addDownloadFiles();
    });
    connect(m_actionShare, &QAction::triggered, this, [=](){
        qDebug() << "分享";
        //分享文件
        dealfile("share");
    });
    connect(m_actionDelete, &QAction::triggered, this, [=](){
        qDebug() << "删除";
        dealfile("delete");
    });
    connect(m_actionProperty, &QAction::triggered, this, [=](){
        qDebug() << "属性";
        dealfile("property");
    });


    connect(m_actionDownloadAsc, &QAction::triggered, this, [=](){
        qDebug() << "按下载量升序";
        getMyFileCount(Asc);
    });
    connect(m_actionDownloadDesc, &QAction::triggered, this, [=](){
        qDebug() << "按下载量降序";
        getMyFileCount(Desc);
    });
    connect(m_actionRefresh, &QAction::triggered, this, [=](){
        qDebug() << "刷新";
        getMyFileCount(Normal);
    });
    connect(m_actionUpload, &QAction::triggered, this, [=](){
        qDebug() << "上传";
        //添加文件到上传任务列表
        addUploadFiles();
    });
}

void MyFileWidget::refreshFiles()
{
    getMyFileCount(MyFileDisplay::Normal);
}

void MyFileWidget::changeUser()
{
    //1. 当切换用户的时候，需要先清空m_fileList
    //2. UploadTask,DownloadTask，上传，下载任务列表也需要清空
    //3. 清空ui-listWidget上面的item.

    this->clearFileList();
    this->clearItems();
    this->clearAllTask();
}

//获取用户文件数量
void MyFileWidget::getMyFileCount(MyFileDisplay cmd)
{
    m_myFilesCount = 0;

    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/myfiles?cmd=count
    QString url = QString("http://%1:%2/myfiles?cmd=count").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject paramsObj;
    paramsObj.insert("user", m_loginInfo->user());
    paramsObj.insert("token", m_loginInfo->token());
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);

    //读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        //读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        QStringList list = NetworkData::getFileCount(data);
        if (!list.isEmpty()) {
            QString code = list.at(0);
            if (code == "110") {  //成功
                m_myFilesCount = list.at(1).toInt();
                qDebug() << "num:" << m_myFilesCount;
            } else if (code == "111") { //token验证失败
                QMessageBox::critical(this, "账号异常", "请重新登录");
                emit sigLoginAgain();
                return;
            }
        }


        if (m_myFilesCount > 0) {
            //请求用户文件信息
            getMyFileList(cmd);
        } else {
            //用户文件数量等于0
            //清空用户文件item
            refreshFileItems();
        }


        //立即销毁
        reply->deleteLater();

    });

}

void MyFileWidget::refreshFileItems()
{
    //清空ui->listWidget中items
    clearItems();

    //如果文件列表不为空，显示文件列表
    if (!m_fileList.isEmpty()) {
        //显示文件列表
        int n = m_fileList.size();
        for (int i=0; i<n; ++i) {
            FileInfo *fileInfo = m_fileList.at(i);
            //添加图标
            addListWidgetItem(fileInfo);
        }
    }

    //添加上传图标
    this->addUploadItem();
}

void MyFileWidget::clearAllTask()
{
    UploadTask *uploadTask = UploadTask::getInstance();
    uploadTask->clearList();
    DownloadTask *downloadTask = DownloadTask::getInstance();
    downloadTask->clearList();
}

//获取用户文件信息
void MyFileWidget::getMyFileList(MyFileDisplay cmd)
{
    QString strCmd;
    if (cmd == MyFileDisplay::Normal) {
        strCmd = "normal";
    } else if (cmd == MyFileDisplay::Asc) {
        strCmd = "pvasc";
    } else if (cmd == MyFileDisplay::Desc) {
        strCmd = "pvdesc";
    }

    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/myfiles?cmd=normal
    //http://192.168.52.139/myfiles?cmd=asc
    //http://192.168.52.139/myfiles?cmd=pvasc
    QString url = QString("http://%1:%2/myfiles?cmd=%3").arg(ip).arg(port).arg(strCmd);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

/*
{
    "count": 1,
    "start": 0,
    "token": "ecf3ac6f8863cd17ed1d3909c4386684",
    "user": "milo"
}
*/
    QJsonObject paramsObj;
    paramsObj.insert("user", m_loginInfo->user());
    paramsObj.insert("token", m_loginInfo->token());
    paramsObj.insert("start", 0);
    paramsObj.insert("count", m_myFilesCount);
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);



    //读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        //读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        //清空m_fileList
        clearFileList();

        m_fileList = NetworkData::getFileInfo(data);
        qDebug() << "m_fileList size()" << m_fileList.size();

        //立即销毁
        reply->deleteLater();

        //清空ui->listWidget中items
        clearItems();

        //在ui->listWidget显示图标
        showFileItems();

    });

}

void MyFileWidget::addUploadItem()
{
    QString filePath = QString(":/images/main/upload.png");
    ui->listWidget->addItem(new QListWidgetItem(QIcon(filePath), QString("上传文件")));
}

//清空了ui->listWidget的items
void MyFileWidget::clearItems()
{
    int count = ui->listWidget->count();
    for (int i=0;i<count;i++) {
        QListWidgetItem *item = ui->listWidget->takeItem(0);  //注意i=0,而不是i=1
        delete item;
    }
}

//清空了m_fileList中的数据
void MyFileWidget::clearFileList()
{
    int size = m_fileList.size();
    for (int i=0;i<size;i++) {
        FileInfo *temp = m_fileList.takeFirst();
        if (temp != nullptr) {
            delete temp;
        }
    }
}

void MyFileWidget::addListWidgetItem(FileInfo *fileInfo)
{
    QString fileTypeName = QString("%1.png").arg(fileInfo->type);
    QString fileName = m_common->getFileType(fileTypeName);
    QString filePath = QString("%1/%2").arg(FILE_TYPE_DIR).arg(fileName);
    qDebug() << "fileName:" << fileName;

    //添加items(图片/文字)到listWidget
    ui->listWidget->addItem(new QListWidgetItem(QIcon(filePath), fileInfo->fileName));
}

void MyFileWidget::showFileItems()
{
    for (int i=0; i<m_fileList.length(); i++) {
        FileInfo *fileInfo = m_fileList.at(i);
        addListWidgetItem(fileInfo);
    }

    //添加上传文件图标
    this->addUploadItem();
}


//分享文件
void MyFileWidget::dealfile(QString cmd)
{
    //获取当前选中的item
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (item == NULL) {
        qDebug() << "选中item NULL";
        return;
    }

    //根据item拿到FileInfo*
    //m_fileList

    for (int i=0; i<m_fileList.length(); i++) {
        FileInfo *fileInfo = m_fileList.at(i);
        //qDebug() << "fileInfo->fileName:" << fileInfo->fileName << "   item->text():" << item->text();
        if (fileInfo->fileName == item->text()) {

            if (cmd == "share") {  //分享文件
                //请求分享文件接口
                shareFile(fileInfo);
            } else if (cmd == "delete") {  //删除文件
                //请求分享文件接口
                deleteFile(fileInfo);
            } else if (cmd == "property") {
                //显示文件信息
                showFileProperty(fileInfo);
            }

            break;
        }
    }
}

//文件分享
void MyFileWidget::shareFile(FileInfo *fileInfo)
{
    QNetworkRequest request;
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/dealfile?cmd=share
    QString url = QString("http://%1:%2/dealfile?cmd=share").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

/*
{
    "filename": "Makefile",
    "md5": "602fdf30db2aacf517badf4565124f51",
    "token": "ecf3ac6f8863cd17ed1d3909c4386684",
    "user": "milo"
}
*/
    QJsonObject paramsObj;
    paramsObj.insert("user", m_loginInfo->user());
    paramsObj.insert("token", m_loginInfo->token());
    paramsObj.insert("filename", fileInfo->fileName);
    paramsObj.insert("md5", fileInfo->md5);
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);

    //读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        //读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        QString code = NetworkData::getCode(data);

/*
010： 成功
011： 失败
012： 别人已经分享此文件
013： token验证失败
*/
        if (code == "010") {  //成功
            fileInfo->shareStatus = 1; //设置此文件已经分享过
            QMessageBox::information(this, "分享成功", QString("【%1】分享成功!").arg(fileInfo->fileName));
        } else if (code == "011") {  //失败
            QMessageBox::warning(this, "分享失败", QString("【%1】分享失败!").arg(fileInfo->fileName));
        } else if (code == "012") {  //别人已经分享此文件
            QMessageBox::warning(this, "分享失败", QString("【%1】别人已经分享此文件!").arg(fileInfo->fileName));
        } else if (code == "013") { //token验证失败
            QMessageBox::critical(this, "账号异常", "请重新登录");
            emit sigLoginAgain();
            return;
        }
    });
}

//文件删除
void MyFileWidget::deleteFile(FileInfo *fileInfo)
{
    QNetworkRequest request;
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/dealfile?cmd=del
    QString url = QString("http://%1:%2/dealfile?cmd=del").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

/*
{
    "filename": "Makefile",
    "md5": "602fdf30db2aacf517badf4565124f51",
    "token": "ecf3ac6f8863cd17ed1d3909c4386684",
    "user": "milo"
}
*/
    QJsonObject paramsObj;
    paramsObj.insert("user", m_loginInfo->user());
    paramsObj.insert("token", m_loginInfo->token());
    paramsObj.insert("filename", fileInfo->fileName);
    paramsObj.insert("md5", fileInfo->md5);
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);

    //读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        //读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        QString code = NetworkData::getCode(data);

/*
013: 成功
004: 失败
*/
        if (code == "013") {  //成功
            QMessageBox::information(this, "文件删除成功", QString("【%1】文件删除成功!").arg(fileInfo->fileName));

            for (int i=0;i<m_fileList.size(); ++i) {
                if (m_fileList.at(i)->fileName == fileInfo->fileName) {
                    //获取对应的 listWidget

                    for (int k=0; k<ui->listWidget->count(); k++) {
                        QListWidgetItem *item = ui->listWidget->item(k);
                        if (item->text() == fileInfo->fileName) {
                            //删除listWidget上的图标
                            ui->listWidget->removeItemWidget(item);
                            delete item;
                            break;
                        }
                    }

                    //删除FileInfo
                    m_fileList.removeAt(i);
                    delete fileInfo;
                    break;
                }
            }

        } else if (code == "014") {  //失败
            QMessageBox::warning(this, "文件删除失败", QString("【%1】文件删除失败!").arg(fileInfo->fileName));
        }
    });
}

void MyFileWidget::showFileProperty(FileInfo *fileInfo)
{
    //显示文件信息
    FilePropertyInfoDialog dlg;
    dlg.setFileInfo(fileInfo);
    dlg.exec();//通过模态对话框显示
}


#include "sharewidget.h"
#include "ui_sharewidget.h"
#include "networkdata.h"
#include <QPainter>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QFileDialog>
#include "FilePropertyInfoDialog.h"
#include <QMessageBox>

ShareWidget::ShareWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareWidget)
{
    ui->setupUi(this);

    initListWidget();

    addMenu();

    m_downloadTask = DownloadTask::getInstance();

    checkTaskList();
}

ShareWidget::~ShareWidget()
{
    delete ui;
}

void ShareWidget::paintEvent(QPaintEvent *event) {
    QPainter p(this);

    QPixmap bg(":/images/main/bk_page.png");
    p.drawPixmap(0,0,this->width(),this->height(), bg);
}

// 初始化ListWidget属性
void ShareWidget::initListWidget()
{
    m_common = Common::getInstance();
    m_loginInfo = LoginInfoInstance::getInstance();
    m_manager = Common::getInstance()->getNetworkAccessManager();

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
    connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &ShareWidget::onRightMenu);
}

void ShareWidget::onRightMenu(const QPoint &pos)
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

void ShareWidget::addMenu()
{
    //点击item显示的菜单
    m_menuItem = new MyMenu(this);

    m_actionDownload = new QAction("下载", this);
    m_actionProperty = new QAction("属性", this);
    m_actionCancel = new QAction("取消分享", this);
    m_actionSave = new QAction("转存文件", this);

    m_menuItem->addAction(m_actionDownload);
    m_menuItem->addAction(m_actionProperty);
    m_menuItem->addAction(m_actionCancel);
    m_menuItem->addAction(m_actionSave);

    //点击空白位置显示的菜单
    m_menuEmpty = new MyMenu(this);
    m_actionRefresh = new QAction("刷新", this);
    m_menuEmpty->addAction(m_actionRefresh);

    menuActions();
}

void ShareWidget::menuActions() {
    connect(m_actionDownload, &QAction::triggered, this, [=](){
        qDebug() << "下载";
        addDownloadFiles();
    });
    connect(m_actionProperty, &QAction::triggered, this, [=](){
        qDebug() << "属性";
        dealfile("property");
    });
    connect(m_actionCancel, &QAction::triggered, this, [=](){
        qDebug() << "取消分享";
        //取消分享
        dealfile("cancel");
    });
    connect(m_actionSave, &QAction::triggered, this, [=](){
        qDebug() << "转存文件";
        dealfile("save");
    });


    connect(m_actionRefresh, &QAction::triggered, this, [=](){
        qDebug() << "刷新";
        getShareFilesCount();
    });

}

//文件属性
void ShareWidget::dealfile(QString cmd)
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
            if (cmd == "property") {
                showFileProperty(fileInfo);
            } else if (cmd == "cancel") {
                cancelShareFile(fileInfo);
            } else if (cmd == "save") {
                saveFileToMyAccount(fileInfo);
            }

            break;
        }
    }
}

void ShareWidget::showFileProperty(FileInfo *fileInfo)
{
    //显示文件信息
    FilePropertyInfoDialog dlg;
    dlg.setFileInfo(fileInfo);
    dlg.exec();//通过模态对话框显示
}

void ShareWidget::cancelShareFile(FileInfo *fileInfo) {
    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/dealsharefile?cmd=cancel
    QString url = QString("http://%1:%2/dealsharefile?cmd=cancel").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

/*
{
    "filename": "ui_buttongroup.h",
    "md5": "a89390d867d5da18c8b1a95908d7c653",
    "user": "milo"
}
*/
    QJsonObject paramsObj;
    paramsObj.insert("user", LoginInfoInstance::getInstance()->user());
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
        if (code == "018") {
            FileInfo *temp = NULL;
            //取消成功
            for (int i=0; i<m_fileList.size(); ++i) {
                temp = m_fileList.at(i);
                if (temp == fileInfo) {
                    QListWidgetItem *item = NULL; //需要删除的图标
                    for (int k=0;k<ui->listWidget->count();k++) { //size count length
                        item = ui->listWidget->item(k);
                        if (item->text() == fileInfo->fileName) {
                            break;
                        }
                    }

                    if (item != NULL) {
                        //删除图标
                        ui->listWidget->removeItemWidget(item);
                        delete item;

                        m_fileList.removeAt(i);
                        delete fileInfo;
                    }

                    break;
                }
            }

            QMessageBox::information(this, "操作成功", "此文件已取消分享!");
        } else if (code == "019") {
            //取消失败
            QMessageBox::warning(this, "操作失败", "取消分享文件操作失败!");
        }

        //立即销毁
        reply->deleteLater();

    });

}

//转存文件
void ShareWidget::saveFileToMyAccount(FileInfo *fileInfo) {
    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/dealsharefile?cmd=save
    QString url = QString("http://%1:%2/dealsharefile?cmd=save").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

/*
{
    "filename": "ui_buttongroup.h",
    "md5": "a89390d867d5da18c8b1a95908d7c653",
    "user": "King"
}
*/
    QString user = LoginInfoInstance::getInstance()->user();

    QJsonObject paramsObj;
    paramsObj.insert("filename", fileInfo->fileName);
    paramsObj.insert("md5", fileInfo->md5);
    paramsObj.insert("user", user);
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
020： 成功
021： 文件已存在
022： 失败
 */
        if (code == "020") {
            //成功
            QMessageBox::information(this, "转存成功", QString("该文件已保存到【%1】文件列表中!").arg(user));

        } else if (code == "021") {
            //文件已存在

            QMessageBox::warning(this, "转存失败", QString("【%1】文件列表中已存在此文件，无需转存!").arg(user));
        } else if (code == "022") {
            //失败

            QMessageBox::warning(this, "转存失败", QString("文件转存失败!").arg(user));
        }

        //立即销毁
        reply->deleteLater();

    });
}


void ShareWidget::checkTaskList()
{
    connect(&m_downloadFileTimer, &QTimer::timeout, this, [=](){
        //定时执行
        //上传文件处理，取出上传任务列表的首任务，上传完后，再取下一个任务
        downloadFilesAction();
    });

    //启动定时器
    m_downloadFileTimer.start(500);
}


void ShareWidget::downloadFilesAction() {
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

    if (downloadFileInfo->isShareTask == false) {
        //不是共享文件任务
        qDebug() << QString("【%1】不是共享文件任务").arg(downloadFileInfo->fileName);
        return;
    }

    QFile *file = downloadFileInfo->file;
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

        m_common->writeRecord(m_loginInfo->user(),
                              downloadFileInfo->fileName,
                              "090");

        //等待
        m_common->sleep(3000);

        //删除下载任务
        m_downloadTask->delDownloadTask();

        //调用共享文件下载标志处理接口
        dealFilePv(downloadFileInfo->md5, downloadFileInfo->fileName);
    });

    //显示文件下载进度
    connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesSent, qint64 bytesTotal){
        downloadFileInfo->fdp->setProgress(bytesSent/1024, bytesTotal/1024);
    });

}

//下载 ,
void ShareWidget::addDownloadFiles()
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
                int res = m_downloadTask->appendDownloadTask(fileInfo, filePath, true);
                if (res == -2) {
                    //下载失败 091
                    m_common->writeRecord(m_loginInfo->user(),
                                          fileInfo->fileName,
                                          "091");
                }
            }
        }
    }
}


void ShareWidget::dealFilePv(QString md5, QString fileName)
{
    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/dealsharefile?cmd=pv
    QString url = QString("http://%1:%2/dealsharefile?cmd=pv").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

/*
{
    "filename": "ui_buttongroup.h",
    "md5": "a89390d867d5da18c8b1a95908d7c653",
    "user": "milo"
}
*/
    QJsonObject paramsObj;
    paramsObj.insert("user", LoginInfoInstance::getInstance()->user());
    paramsObj.insert("filename", fileName);
    paramsObj.insert("md5", md5);
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);


    //读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        //读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        QString code = NetworkData::getCode(data);
        if (code == "017") {
            //成功
            qDebug() <<"PV处理成功";

            //需要去改m_fileList中对应的文件pv值
            for (int i=0;i<m_fileList.size();++i) {
                FileInfo *info = m_fileList.at(i);
                if (info->md5 == md5 && info->fileName == fileName) {
                    int pv = info->pv;
                    info->pv = pv + 1;
                    break;
                }
            }

        } else {
            //失败
            qDebug() <<"PV处理失败";
        }

        //立即销毁
        reply->deleteLater();

    });

}

void ShareWidget::refreshFiles()
{
    getShareFilesCount();
}

//获取共享文件数量
void ShareWidget::getShareFilesCount()
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

        m_shareFilesCount = NetworkData::getCount(data);
        if (m_shareFilesCount > 0) {
            //请求用户文件信息
            getShareFilesList();
        } else {
            //共享文件数量等于0
            refreshFileItems();
        }

        //立即销毁
        reply->deleteLater();

    });

}

//获取共享文件信息
void ShareWidget::getShareFilesList()
{
    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/sharefiles?cmd=normal
    QString url = QString("http://%1:%2/sharefiles?cmd=normal").arg(ip).arg(port);
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

//清空了ui->listWidget的items
void ShareWidget::clearItems()
{
    int count = ui->listWidget->count();
    for (int i=0;i<count;i++) {
        QListWidgetItem *item = ui->listWidget->takeItem(0);  //注意i=0,而不是i=1
        delete item;
    }
}

//清空了m_fileList中的数据
void ShareWidget::clearFileList()
{
    int size = m_fileList.size();
    for (int i=0;i<size;i++) {
        FileInfo *temp = m_fileList.takeFirst();
        if (temp != nullptr) {
            delete temp;
        }
    }
}

void ShareWidget::showFileItems()
{
    for (int i=0; i<m_fileList.length(); i++) {
        FileInfo *fileInfo = m_fileList.at(i);
        addListWidgetItem(fileInfo);
    }
}

void ShareWidget::addListWidgetItem(FileInfo *fileInfo)
{
    QString fileTypeName = QString("%1.png").arg(fileInfo->type);
    QString fileName = m_common->getFileType(fileTypeName);
    QString filePath = QString("%1/%2").arg(FILE_TYPE_DIR).arg(fileName);
    qDebug() << "fileName:" << fileName;

    //添加items(图片/文字)到listWidget
    ui->listWidget->addItem(new QListWidgetItem(QIcon(filePath), fileInfo->fileName));
}

// 刷新listWidget列表
void ShareWidget::refreshFileItems()
{
    // 清空所有item项目
    clearItems();

    int n = m_fileList.size();
    for(int i=0; i<n; ++i) {
        FileInfo *fileInfo = m_fileList.at(i);
        addListWidgetItem(fileInfo);
    }
}


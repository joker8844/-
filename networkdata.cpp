#include "networkdata.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>

NetworkData::NetworkData(QObject *parent) : QObject(parent)
{

}

QString NetworkData::getCode(QByteArray json)
{
    QString code;
    QJsonParseError err;
    //解析返回数据
    QJsonDocument rootDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "Json格式错误";
    } else {
        //解析json
        QJsonObject rootObj = rootDoc.object();
        QJsonValue codeValue = rootObj.value("code");
        code = codeValue.toString();
    }

    return code;
}

int NetworkData::getCount(QByteArray json)
{
    int count;
    QJsonParseError err;
    //解析返回数据
    QJsonDocument rootDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "Json格式错误";
    } else {
        //解析json
        QJsonObject rootObj = rootDoc.object();
        QJsonValue codeValue = rootObj.value("count");
        count = codeValue.toInt();
    }

    return count;
}

QStringList NetworkData::getFileCount(QByteArray json)
{
    QStringList list;
    QJsonParseError err;
    //解析返回数据
    QJsonDocument rootDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "Json格式错误";
    } else {
        //解析json
        QJsonObject rootObj = rootDoc.object();
        QJsonValue codeValue = rootObj.value("code");

        /*
        {
            "num":	"1",
            "code":	"110"
        }
        */

        if (codeValue.type() == QJsonValue::String) {
            QString code = codeValue.toString();
            list.append(code);
        }
        QJsonValue numValue = rootObj.value("num");
        if (numValue.type() == QJsonValue::String) {
            QString num = numValue.toString();
            list.append(num);
        }
    }

    return list;
}

QList<FileInfo*> NetworkData::getFileInfo(QByteArray json)
{
    QList<FileInfo*> list;
    QJsonParseError err;
    //解析返回数据
    QJsonDocument rootDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "Json格式错误";
    } else {
        //解析json
        QJsonObject rootObj = rootDoc.object();
        QJsonValue filesValue = rootObj.value("files");
        if (filesValue.type() == QJsonValue::Array) {
            QJsonArray filesArr = filesValue.toArray();

            for (int i=0;i<filesArr.count();i++) {
                QJsonValue fileValue = filesArr.at(i);
                if (fileValue.type() == QJsonValue::Object) {
                    FileInfo *fileInfo = new FileInfo;

                    QJsonObject fileObj = fileValue.toObject();
                    QJsonValue userValue = fileObj.value("user");
                    QJsonValue md5Value = fileObj.value("md5");
                    QJsonValue createTimeValue = fileObj.value("create_time");
                    QJsonValue fileNameValue = fileObj.value("file_name");
                    QJsonValue shareStatus = fileObj.value("share_status");
                    QJsonValue pvValue = fileObj.value("pv");
                    QJsonValue urlValue = fileObj.value("url");
                    QJsonValue sizeValue = fileObj.value("size");
                    QJsonValue typeValue = fileObj.value("type");

                    fileInfo->user = userValue.toString();
                    fileInfo->md5 = md5Value.toString();
                    fileInfo->createTime = createTimeValue.toString();
                    fileInfo->fileName = fileNameValue.toString();
                    fileInfo->shareStatus = shareStatus.toInt();
                    fileInfo->pv = pvValue.toInt();
                    fileInfo->url = urlValue.toString();
                    fileInfo->size = sizeValue.toInt();
                    fileInfo->type = typeValue.toString();

                    list.append(fileInfo);
                }
            }
        }
    }

    return list;
}


QList<FileDownload*> NetworkData::getFileDownload(QByteArray json)
{
    QList<FileDownload*> list;
    QJsonParseError err;
    //解析返回数据
    QJsonDocument rootDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "Json格式错误";
    } else {
        //解析json
        QJsonObject rootObj = rootDoc.object();
        QJsonValue filesValue = rootObj.value("files");
        if (filesValue.type() == QJsonValue::Array) {
            QJsonArray filesArr = filesValue.toArray();

            for (int i=0;i<filesArr.count();i++) {
                QJsonValue fileValue = filesArr.at(i);
                if (fileValue.type() == QJsonValue::Object) {
                    FileDownload *fileDownload = new FileDownload;

                    QJsonObject fileObj = fileValue.toObject();
                    QJsonValue fileNameValue = fileObj.value("filename");
                    QJsonValue pvValue = fileObj.value("pv");

                    fileDownload->fileName = fileNameValue.toString();
                    fileDownload->pv = pvValue.toInt();

                    list.append(fileDownload);
                }
            }
        }
    }

    return list;
}

#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>

/*
 *{
    "files":	[{
            "user":	"milo",
            "md5":	"7474b5b4ce9b5fc93740758a3734b195",
            "create_time":	"2020-10-31 01:53:48",
            "file_name":	"libvtv-0.dll",
            "share_status":	0,
            "pv":	0,
            "url":	"http://192.168.139.131:80/group1/M00/00/00/wKiLg1-dJhyACXI_AAA6DiX82dA943.dll",
            "size":	14862,
            "type":	"dll"
        }]
}*/
struct FileInfo
{
    QString user;           //用户名
    QString md5;            //md5
    QString createTime;     //上传时间
    QString fileName;       //文件名称
    int shareStatus;        //共享状态, 0为没有共享， 1为共享
    int pv;                 //文件下载量，下载一次加1
    QString url;            //文件url
    int size;               //文件大小,单位:字节
    QString type;           //文件类型
};


struct FileDownload
{
    QString fileName;       //文件名称
    int pv;                 //文件下载量，下载一次加1
};

#endif // FILEINFO_H

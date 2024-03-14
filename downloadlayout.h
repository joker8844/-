#ifndef DOWNLOADLAYOUT_H
#define DOWNLOADLAYOUT_H

#include <QWidget>
#include <QVBoxLayout>

//下载进度布局类, 单例模式
class DownloadLayout
{
public:
    static DownloadLayout* getInstance();
    void setDownloadLayout(QWidget *parent);

    //获取下载的布局变量
    QVBoxLayout* getDownloadLayout();

private:
    DownloadLayout();
    ~DownloadLayout();


    static DownloadLayout *m_instance;

    QVBoxLayout *m_vLayout;
};

#endif // DOWNLOADLAYOUT_H

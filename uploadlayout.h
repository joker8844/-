#ifndef UPLOADLAYOUT_H
#define UPLOADLAYOUT_H

#include <QWidget>
#include <QVBoxLayout>

//上传进度布局类, 单例模式
class UploadLayout
{
public:
    static UploadLayout* getInstance();
    void setUploadLayout(QWidget *parent);

    //获取上传的布局变量
    QVBoxLayout* getUploadLayout();

private:
    UploadLayout();
    ~UploadLayout();


    static UploadLayout *m_instance;

    QVBoxLayout *m_vLayout;
};

#endif // UPLOADLAYOUT_H

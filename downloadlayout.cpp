#include "downloadlayout.h"
#include <QLayout>

DownloadLayout *DownloadLayout::m_instance = new DownloadLayout;

DownloadLayout::DownloadLayout()
{

}

DownloadLayout::~DownloadLayout()
{

}


DownloadLayout* DownloadLayout::getInstance()
{
    return m_instance;
}

void DownloadLayout::setDownloadLayout(QWidget *parent)
{
    //parent download_scroll
    //使用垂直布局
    QVBoxLayout *vLayout = new QVBoxLayout;
    m_vLayout = vLayout;
    //添加弹簧
    m_vLayout->addStretch();
    parent->setLayout(vLayout);
}

QVBoxLayout* DownloadLayout::getDownloadLayout() {
    return m_vLayout;
}

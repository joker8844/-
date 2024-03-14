#include "uploadlayout.h"
#include <QLayout>

UploadLayout *UploadLayout::m_instance = new UploadLayout;

UploadLayout::UploadLayout()
{

}

UploadLayout::~UploadLayout()
{

}


UploadLayout* UploadLayout::getInstance()
{
    return m_instance;
}

void UploadLayout::setUploadLayout(QWidget *parent)
{
    //parent upload_scroll
    //使用垂直布局
    QVBoxLayout *vLayout = new QVBoxLayout;
    m_vLayout = vLayout;
    //添加弹簧
    m_vLayout->addStretch();
    parent->setLayout(vLayout);
}

QVBoxLayout* UploadLayout::getUploadLayout() {
    return m_vLayout;
}

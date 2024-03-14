#include "mymenu.h"

MyMenu::MyMenu(QWidget *parent) : QMenu(parent)
{
    //设置样式
    this->setStyleSheet(
        "color: rgb(236, 72, 39);"
        "font: 14pt \"隶书\";"
        " background-color: rgb(102, 216, 105);"
    );
}

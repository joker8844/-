#ifndef MYMENU_H
#define MYMENU_H

#include <QMenu>
#include <QWidget>

class MyMenu : public QMenu
{
    Q_OBJECT
public:
    explicit MyMenu(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // MYMENU_H

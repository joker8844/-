#ifndef BUTTONGROUP_H
#define BUTTONGROUP_H

#include <QWidget>
#include <QMouseEvent>
#include <QToolButton>
#include <QMap>
#include <QSignalMapper>

namespace Ui {
class ButtonGroup;
}

class ButtonGroup : public QWidget
{
    Q_OBJECT

public:
    explicit ButtonGroup(QWidget *parent = 0);
    ~ButtonGroup();
    void setParent(QWidget *parent);
    void setUser(QString user);
    void defaultPage();

protected:
    void paintEvent(QPaintEvent *event);
    //鼠标拖动
    void mouseMoveEvent(QMouseEvent *event);
    //鼠标按下
    void mousePressEvent(QMouseEvent *event);

signals:
    void minWindow();
    void maxWindow();
    void closeWindow();

    void sigMyFile();
    void sigShare();
    void sigDownload();
    void sigTransform();
    void sigSwitchUser();

public slots:
    void onMapperButtonsClicked(QString text);

private:
    Ui::ButtonGroup *ui;

    QWidget *m_parent;
    QPoint m_pt;
    QToolButton *m_curBtn;  //选中按钮

    QMap<QString, QToolButton*> m_map;
    QSignalMapper *m_mapper;
};

#endif // BUTTONGROUP_H

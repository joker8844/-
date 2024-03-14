#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
#include <QPoint>

namespace Ui {
class TitleWidget;
}

class TitleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TitleWidget(QWidget *parent = 0);
    ~TitleWidget();

protected:
    //鼠标拖动
    void mouseMoveEvent(QMouseEvent *event);
    //鼠标按下
    void mousePressEvent(QMouseEvent *event);

signals:
    void showSetPage();
    void closeButton();

private slots:
    void on_close_clicked();

    void on_min_clicked();

    void on_set_clicked();

private:
    Ui::TitleWidget *ui;

    QPoint m_pt;
    QWidget *m_parent;
};

#endif // TITLEWIDGET_H

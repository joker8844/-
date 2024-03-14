#ifndef TRANSFORMWIDGET_H
#define TRANSFORMWIDGET_H

#include <QWidget>
#include "common.h"

namespace Ui {
class TransformWidget;
}

class TransformWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransformWidget(QWidget *parent = 0);
    ~TransformWidget();

    void showPage(TransformStatus status);
private slots:
    void on_clear_clicked();

private:
    Ui::TransformWidget *ui;

    void dispayDataRecord(QString path=RECORD_DIR);

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // TRANSFORMWIDGET_H

#ifndef FILEDATAPROGRESS_H
#define FILEDATAPROGRESS_H

#include <QWidget>

namespace Ui {
class FileDataProgress;
}

class FileDataProgress : public QWidget
{
    Q_OBJECT

public:
    explicit FileDataProgress(QWidget *parent = 0);
    ~FileDataProgress();


    void setFileName(QString fileName);
    void setProgress(int value, int max);

private:
    Ui::FileDataProgress *ui;
};

#endif // FILEDATAPROGRESS_H

#ifndef FILEPROPERTYINFODIALOG_H
#define FILEPROPERTYINFODIALOG_H

#include <QDialog>
#include "networkdata.h"

namespace Ui {
class FilePropertyInfoDialog;
}

class FilePropertyInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilePropertyInfoDialog(QWidget *parent = 0);
    ~FilePropertyInfoDialog();
    void setFileInfo(FileInfo *fileInfo);

private:
    Ui::FilePropertyInfoDialog *ui;
};

#endif // FILEPROPERTYINFODIALOG_H

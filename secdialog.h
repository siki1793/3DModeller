#ifndef SECDIALOG_H
#define SECDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include "model.h"
// #include "mainwindow.h"


namespace Ui {
class secDialog;
}

class secDialog : public QDialog
{
    Q_OBJECT

public:
    explicit secDialog(QWidget *parent = 0);
    ~secDialog();
    void window(vtkSmartPointer<vtkRenderer>);

private slots:
    void on_pushButton_clicked();

private:
    Ui::secDialog *ui;
};

#endif // SECDIALOG_H

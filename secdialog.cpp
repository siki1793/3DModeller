#include "secdialog.h"
#include "ui_secdialog.h"


secDialog::secDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::secDialog)
{
    ui->setupUi(this);

}

secDialog::~secDialog()
{
    delete ui;
}

void secDialog::on_pushButton_clicked()
{
    QMessageBox::information(this,
                                 "Your are in sec window hahaha",
                                 tr("Done!"));
}

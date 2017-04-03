#include "secdialog.h"
#include "ui_secdialog.h"

// MainWindow m;
model mo;


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
	QString savefile=QFileDialog::getSaveFileName(this,tr("save as"),"","STL (*.stl);;All Files(*.*)");
    QFile file(savefile);
    if(!savefile.isEmpty())
    {
       
        if(file.open(QFile::WriteOnly))
        {
                  // Write to a STL File
            vtkSmartPointer<vtkSTLWriter> stlWriter =   vtkSmartPointer<vtkSTLWriter>::New();
            stlWriter=mo.saveFile(savefile);
            stlWriter->Write();
            QMessageBox::information(this,
                             "File Save",
                             tr("Done!"));
        }
        else
        {
            QMessageBox::warning(
                        this,
                        "save file",
                        tr("cannot save the file %1. \n Error %2")
                        .arg(savefile)
                        .arg(file.errorString()));
        }
    }

}

void secDialog::window(vtkSmartPointer<vtkRenderer> renderer)
{
	ui->verticalWidget_3->GetRenderWindow()->AddRenderer(renderer);
    cout<<"SecDialog"<<endl;
}

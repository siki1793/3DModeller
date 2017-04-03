#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include "secdialog.h"
#include "model.h"
#include "vtkCommand.h"
#include "vtkCamera.h"
//#include <QVTKWidget.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindow.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    //vtkRenderer* ren;
    ~MainWindow();

private slots:
    void on_openButton_clicked();

    void on_generateVolumeButton_clicked();

    void slicesGenerate(double ,double );
    
    void volumeGenerate(double ,double ,double ,double ,double ,double ,bool );
    
    void surfaceGenerate(double ,double );
    
    void setIsoValues(int );

    void on_pushButton_3_clicked();

    void on_generateFinalSurface_clicked();

    void on_resetButton_clicked();

    void on_ELSButton_clicked();

    void on_clipCheckBox_stateChanged(int);

    void on_checkBox_2_stateChanged(int);

    void on_checkBox_3_stateChanged(int);

    void on_checkBox_4_stateChanged(int);

    void setLimits();

    //void slicesView(double,double);
private:
    Ui::MainWindow *ui;
    secDialog *sec;

};

#endif // MAINWINDOW_H

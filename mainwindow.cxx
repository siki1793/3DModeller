#include "mainwindow.h"
#include "ui_mainwindow.h"

vtkSmartPointer<vtkRenderer> volrenderer= vtkSmartPointer<vtkRenderer>::New();
vtkSmartPointer<vtkRenderer> surfacerenderer= vtkSmartPointer<vtkRenderer>::New();

vtkSmartPointer<vtkRenderer> renderer =  vtkSmartPointer<vtkRenderer>::New();

double isoValStart=177.00;
double isoValEnd=3071.00;

model m;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//open file and initialize surface volume and slices surface is created at the time of opening the files for clipping the volume.
void MainWindow::on_openButton_clicked()
{
    QString filename=QFileDialog::getExistingDirectory(this,tr("Open directory"),"\\home",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    //DICOM reader will read the set of dicom files and copied to image data.
    m.openFile(filename);
    
    //here the surface is initialized i.e make an another copy of the surface as mainsurface and temp surface.
    m.surfaceInitialize();

    //marching cubes done to the image data.
	m.doMarchingcubes(isoValStart,isoValEnd);
    //copy the imagedata to poly data if clipping is done the clipping is applied to the poly data and shows on the screen.
	m.generateSurface(0.93, 0.25, 0.30,1.0,surfacerenderer);

    //copy the dicom data to vtk smart volume mapper.
    m.volumeInitialize();
    //calls the volume generate checks is override is there are not.
    volumeGenerate(isoValStart,isoValEnd,0.93, 0.25, 0.30,1.0,true);
 	
 	cout<<"Before Slices"<<endl;   
    //set the limits of the dicom files.
    m.calculateLimits();
    //sets the imagedata to imageReader2 for doing operations on slices.
    m.slicesInitialization(m.getLimits(1),m.getLimits(3),m.getLimits(5));
 	
 	cout<<"After Slices Initialization"<<endl;   
    //initialize Axial, Coronal and Sagittal planes.
    slicesGenerate(isoValStart,isoValEnd);
 	cout<<"After Slices Generation"<<endl;   

    QMessageBox::information(this,tr("File Open"),tr("Done!! :)"));
}

void MainWindow::surfaceGenerate(double isoValStart,double isoValEnd)
{
	m.doMarchingcubes(isoValStart,isoValEnd);
	m.generateSurface(0.93, 0.25, 0.30,1.0,surfacerenderer);
	ui->clipCheckBox->setCheckState(Qt::Unchecked);
    //call boxCreation method.
	m.boxCreation(volrenderer,false);
	cout<<"Surface Generation Done"<<endl;
}

void MainWindow::slicesGenerate(double isoValStart,double isoValEnd)
{
	vtkSmartPointer<vtkRenderer> sagittalRenderer= vtkSmartPointer<vtkRenderer>::New();
	ui->verticalWidget_2->GetRenderWindow()->AddRenderer(sagittalRenderer);	
    //generate the sagittal plane.
	m.slicesGeneration("sagittal",isoValStart,isoValEnd,sagittalRenderer);
	sagittalRenderer->Render();
	cout<<"Sagittal Slices Generation Done"<<endl;

	vtkSmartPointer<vtkRenderer> coronalRenderer= vtkSmartPointer<vtkRenderer>::New();
	ui->verticalWidget_3->GetRenderWindow()->AddRenderer(coronalRenderer);	
    //generate the coronal plane.
	m.slicesGeneration("coronal",isoValStart,isoValEnd,coronalRenderer);
	coronalRenderer->Render();

	vtkSmartPointer<vtkRenderer> axialRenderer= vtkSmartPointer<vtkRenderer>::New();
	ui->verticalWidget_4->GetRenderWindow()->AddRenderer(axialRenderer);	
    //generate the axial plane.
	m.slicesGeneration("axial",isoValStart,isoValEnd,axialRenderer);
	axialRenderer->Render();
	cout<<"Slices Generation Done"<<endl;
}

void MainWindow::volumeGenerate(double isoValueStart,double isoValueEnd,double red,double green,double blue,double opacity,bool overwrite)
{
    //Volume Rendering
    ui->verticalWidget->GetRenderWindow()->AddRenderer(volrenderer);
    //checks if overwrite surface is selected or not.
    if(ui->overwriteCheckbox->isChecked())
    {
		cout<<"		Inside Volume Generation Overwrite "<<endl;

	    m.generateVolume(isoValStart,isoValEnd,red, green, blue,opacity,true,volrenderer);
    }
    else
    {
	    m.generateVolume(isoValStart,isoValEnd,red, green, blue,opacity,false,volrenderer);
    }    
    volrenderer->Render();
	cout<<"		Volume Generation Done"<<endl;
}
void MainWindow::on_generateVolumeButton_clicked()
{
    //select the iso values for start and end point/position.
	setIsoValues(ui->volumeSelectionComboBox->currentIndex());
	cout<<"		Setting of IsoValues Done"<<endl;
    volumeGenerate(isoValStart,isoValEnd,0.93, 0.25, 0.30,1.0,true);
    slicesGenerate(isoValStart,isoValEnd);

    QMessageBox::information(this,"Volume Generation", tr("Done!"));
   
}
void MainWindow::on_clipCheckBox_stateChanged(int arg1)//clipping box
{
    if(ui->clipCheckBox->isChecked())
    {
        m.boxCreation(volrenderer,true);
    }
    else
    {
        m.boxCreation(volrenderer,false);
    }
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)//with verticalLayout_5
{
    if(ui->checkBox_2->isChecked())
    {

    }
    else
    {
        
    }
}

void MainWindow::on_checkBox_3_stateChanged(int arg1)//with verticalLayout_6
{
    if(ui->checkBox_3->isChecked())
    {

    }
    else
    {
        
    }
    
}

void MainWindow::on_checkBox_4_stateChanged(int arg1)//with verticalLayout_7
{
    if(ui->checkBox_4->isChecked())
    {

    }
    else
    {
        
    }
}


void MainWindow::on_generateFinalSurface_clicked()
{
    //surface is generated and the render is send to the secondery window.
    surfaceGenerate(isoValStart,isoValEnd);

    sec =new secDialog(this);
    sec->window(surfacerenderer);
    cout<<"Sec Window"<<endl;
    sec->show();
}


void MainWindow::on_pushButton_3_clicked()
{
    m.doMarchingcubes(isoValStart,isoValEnd);

    vtkSmartPointer<vtkActor> actor=vtkSmartPointer<vtkActor>::New();
    actor=m.actorCreation(false);
    renderer->RemoveAllViewProps();
    renderer->Render();
    renderer->AddActor(actor);

    ui->verticalWidget->GetRenderWindow()->AddRenderer(renderer);
    QMessageBox::information(this,
                             "Rendering File",
                             tr("Done!"));
}

void MainWindow::on_ELSButton_clicked()
{
    vtkSmartPointer<vtkActor> actor=vtkSmartPointer<vtkActor>::New();
    actor=m.actorCreation(true);
    std::cout<<"inside on_ELSButton_clicked"<<std::endl;
    renderer->RemoveAllViewProps();
    renderer->Render();
    renderer->AddActor(actor);
    ui->verticalWidget->GetRenderWindow()->AddRenderer(renderer);
        QMessageBox::information(this,
                             "largest surface extracted",
                             tr("Done!"));
}

void MainWindow::on_resetButton_clicked()
{
    m.doReset();
    m.doMarchingcubes(isoValStart,isoValEnd);
    vtkSmartPointer<vtkActor> actor=vtkSmartPointer<vtkActor>::New();
    actor=m.actorCreation(false);
    renderer->RemoveAllViewProps();
    renderer->Render();
    renderer->AddActor(actor);
    setLimits();
    ui->verticalWidget->GetRenderWindow()->AddRenderer(renderer);
        QMessageBox::information(this,
                             "Reset Volume",
                             tr("Done!"));
}

//set limits of the dicom files.
void MainWindow::setLimits()
{
        QString xLimit=QString::number(m.limits[0])+" - "+QString::number(m.limits[1]);
        // ui->xLimitLabel->setText(xLimit);
        QString yLimit=QString::number(m.limits[2])+" - "+QString::number(m.limits[3]);
        // ui->yLimitLabel->setText(yLimit);
        QString zLimit=QString::number(m.limits[4])+" - "+QString::number(m.limits[5]);
        // ui->zLimitLabel->setText(zLimit);
}

vtkSmartPointer<vtkRenderer> getRenderer(void)
{
    return renderer;
}


void MainWindow::setIsoValues(int index)
{
	switch(index)
	{
		case 1://iso values for bone.
				isoValStart=226;
    			isoValEnd=3071;
    			break;	

		case 2://iso values for Compact Bone (Adult).
				isoValStart=662;
    			isoValEnd=1988;
    			break;	

		case 3://iso values for Compact Bone (Child).
				isoValStart=586;
    			isoValEnd=2198;
    			break;	

		case 4://iso values for custom values.
            	QMessageBox::information(this,
                             "select from sliders",
                             tr("Done!"));
           			break;	

		case 5://iso values for Enamel (Adult)
				isoValStart=1553;
    			isoValEnd=2850;
    			break;	

		case 6://iso values for Enamel (Child)
				isoValStart=2042;
    			isoValEnd=3071;
    			break;	

		case 7://iso values for Fat Tissue (Adult)
				isoValStart=-205;
    			isoValEnd=-51;
    			break;	

		case 8://iso values for Fat Tissue (Child)
				isoValStart=-212;
    			isoValEnd=-72;
    			break;	

		case 9://iso values for Muscle Tissue (Adult)
				isoValStart=-5;
    			isoValEnd=135;
    			break;	

		case 10://iso values for Muscle Tissue (Child)
				isoValStart=-25;
    			isoValEnd=139;
    			break;	

		case 11://iso values for Skin Tissue (Adult)
				isoValStart=-718;
    			isoValEnd=-177;
    			break;	

		case 12://iso values for Skin Tissue (Child)
				isoValStart=-766;
    			isoValEnd=-202;
    			break;	

		case 13://iso values for Soft Tissue
				isoValStart=-700;
    			isoValEnd=225;
    			break;	

		case 14://iso values for Spongial Bone (Adult)
				isoValStart=148;
    			isoValEnd=661;
    			break;	

		case 15://iso values for Spongial Bone (Child)
				isoValStart=156;
    			isoValEnd=585;
    			break;	
	}
}

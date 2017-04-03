#include "model.h"

double spacing[3];
double origin[3];

vtkSmartPointer<vtkDICOMImageReader> dicomReader =  vtkSmartPointer<vtkDICOMImageReader>::New();
vtkSmartPointer<vtkImageReader2> slicesReader = vtkSmartPointer<vtkImageReader2>::New();
vtkSmartPointer<vtkImageData> surface = vtkSmartPointer<vtkImageData>::New();
vtkSmartPointer<vtkImageData> mainSurface = vtkSmartPointer<vtkImageData>::New();
vtkSmartPointer<vtkMarchingContourFilter> marchingCubes = vtkSmartPointer<vtkMarchingContourFilter>::New();
vtkSmartVolumeMapper *volmapper = vtkSmartVolumeMapper::New();
vtkSmartVolumeMapper *tempvolmapper = vtkSmartVolumeMapper::New();
vtkSmartPointer<vtkPolyDataMapper> mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
vtkSmartPointer<vtkPolyData> polydata= vtkSmartPointer<vtkPolyData>::New();
vtkSmartPointer<vtkBoxWidget> box=vtkSmartPointer<vtkBoxWidget>::New();

vtkSmartPointer<vtkFillHolesFilter> fillHolesFilter = vtkSmartPointer<vtkFillHolesFilter>::New();
vtkSmartPointer<vtkPlanes> planes = vtkSmartPointer<vtkPlanes>::New();
vtkSmartPointer<vtkClipClosedSurface> clipClosedSurface1 = vtkSmartPointer<vtkClipClosedSurface>::New();
vtkSmartPointer<vtkClipClosedSurface> clipClosedSurface2 = vtkSmartPointer<vtkClipClosedSurface>::New();
vtkSmartPointer<vtkClipClosedSurface> clipClosedSurface3 = vtkSmartPointer<vtkClipClosedSurface>::New();
vtkSmartPointer<vtkClipClosedSurface> clipClosedSurface4 = vtkSmartPointer<vtkClipClosedSurface>::New();
vtkSmartPointer<vtkClipClosedSurface> clipClosedSurface5 = vtkSmartPointer<vtkClipClosedSurface>::New();
vtkSmartPointer<vtkClipClosedSurface> clipClosedSurface6 = vtkSmartPointer<vtkClipClosedSurface>::New();


vtkSmartPointer<vtkImageData> volumeMC = vtkSmartPointer<vtkImageData>::New();

vtkSmartPointer<vtkPolyData> poly=vtkSmartPointer<vtkPolyData>::New();
vtkSmartPointer<vtkPolyData> polytemp=vtkSmartPointer<vtkPolyData>::New();

class vtkBoxWidgetCallback : public vtkCommand
{
public:
  static vtkBoxWidgetCallback *New()
    { return new vtkBoxWidgetCallback; }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
      if (this->Mapper)
        {
        widget->GetPlanes(planes);
        this->Mapper->SetClippingPlanes(planes);
//        mapper->SetClippingPlanes(planes);
//        mapper->Update();
//        planes->Delete();
        }
    }
  void SetMapper(vtkSmartVolumeMapper* m)
    { this->Mapper = m; }

protected:
  vtkBoxWidgetCallback()
    { this->Mapper = 0; }

  vtkSmartVolumeMapper *Mapper;
};


// The mouse motion callback, to turn "Slicing" on and off
class vtkImageInteractionCallback : public vtkCommand
{
public:

  static vtkImageInteractionCallback *New() {
    return new vtkImageInteractionCallback; };

  vtkImageInteractionCallback() {
    this->Slicing = 0;
    this->ImageReslice = 0;
    this->Interactor = 0; };

  void SetImageReslice(vtkImageReslice *reslice) {
    this->ImageReslice = reslice; };

  vtkImageReslice *GetImageReslice() {
    return this->ImageReslice; };

  void SetInteractor(vtkRenderWindowInteractor *interactor) {
    this->Interactor = interactor; };

  vtkRenderWindowInteractor *GetInteractor() {
    return this->Interactor; };

  virtual void Execute(vtkObject *, unsigned long event, void *)
    {
    vtkRenderWindowInteractor *interactor = this->GetInteractor();

    int lastPos[2];
    interactor->GetLastEventPosition(lastPos);
    int currPos[2];
    interactor->GetEventPosition(currPos);

    if (event == vtkCommand::LeftButtonPressEvent)
      {
      this->Slicing = 1;
      }
    else if (event == vtkCommand::LeftButtonReleaseEvent)
      {
      this->Slicing = 0;
      }
    else if (event == vtkCommand::MouseMoveEvent)
      {
      if (this->Slicing)
        {
        vtkImageReslice *reslice = this->ImageReslice;

        // Increment slice position by deltaY of mouse
        int deltaY = lastPos[1] - currPos[1];

        reslice->Update();
        double sliceSpacing = reslice->GetOutput()->GetSpacing()[2];
        vtkMatrix4x4 *matrix = reslice->GetResliceAxes();
        // move the center point that we are slicing through
        double point[4];
        double center[4];
        point[0] = 0.0;
        point[1] = 0.0;
        point[2] = sliceSpacing * deltaY;
        point[3] = 1.0;
        matrix->MultiplyPoint(point, center);
        matrix->SetElement(0, 3, center[0]);
        matrix->SetElement(1, 3, center[1]);
        matrix->SetElement(2, 3, center[2]);
        interactor->Render();
        }
      else
        {
        vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(
          interactor->GetInteractorStyle());
        if (style)
          {
          style->OnMouseMove();
          }
        }
      }
    };

private:

  // Actions (slicing only, for now)
  int Slicing;

  // Pointer to vtkImageReslice
  vtkImageReslice *ImageReslice;

  // Pointer to the interactor
  vtkRenderWindowInteractor *Interactor;
};


void model::openFile(QString filename)
{
    QByteArray ba = filename.toLatin1();
    const char *c_str1 = ba.data(); 
    dicomReader->SetDirectoryName(c_str1);
    dicomReader->Update();
}

void model::surfaceInitialize()
{
    mainSurface->DeepCopy(dicomReader->GetOutput());
    surface=mainSurface;
}

//extract volume of interest to the surface.
void model::extractVOI()
{
   double* bounds;
    bounds=planes->GetPoints()->GetBounds();
  
  std::cout  << "xmin: " << bounds[0] << " " 
             << "xmax: " << bounds[1] << std::endl
             << "ymin: " << bounds[2] << " " 
             << "ymax: " << bounds[3] << std::endl
             << "zmin: " << bounds[4] << " " 
             << "zmax: " << bounds[5] << std::endl;
    
    double index[6];
    index[0]=(bounds[0]/spacing[0])-origin[0];
    index[1]=(bounds[1]/spacing[0])-origin[0];
    index[2]=(bounds[2]/spacing[1])-origin[1];
    index[3]=(bounds[3]/spacing[1])-origin[1];
    index[4]=(bounds[4]/spacing[2])-origin[2];
    index[5]=(bounds[5]/spacing[2])-origin[2];
    std::cout  << "xmin: " << index[0] << " " 
             << "xmax: " << index[1] << std::endl
             << "ymin: " << index[2] << " " 
             << "ymax: " << index[3] << std::endl
             << "zmin: " << index[4] << " " 
             << "zmax: " << index[5] << std::endl;

    if(index[0]<limits[0])
    {
        index[1]=index[1]+index[0];
        index[0]=0;
    }
    if(index[2]<limits[2])
    {
        index[3]=index[2]+index[3];
        index[2]=0;
    }
    if(index[4]<limits[4])
    {
        index[5]=index[4]+index[5];
        index[4]=0;
    }
    if(index[1]>limits[1])
    {
        index[0]=index[0]-(index[1]-limits[1]);
        index[1]=limits[1];
        index[0]<0?index[0]=limits[0]:index[0];
    }
    if(index[3]>limits[3])
    {
        index[2]=index[2]-(index[3]-limits[3]);
        index[3]=limits[3];
        index[2]<0?index[2]=limits[2]:index[2];
    }
    if(index[5]>limits[5])
    {
        index[4]=index[4]-(index[5]-limits[5]);
        index[5]=limits[5];
        index[4]<0?index[4]=limits[4]:index[4];
    }
    std::cout  << "xmin: " << index[0] << " " 
             << "xmax: " << index[1] << std::endl
             << "ymin: " << index[2] << " " 
             << "ymax: " << index[3] << std::endl
             << "zmin: " << index[4] << " " 
             << "zmax: " << index[5] << std::endl;
    
    cout<<"Before Extract VOI"<<endl;

    vtkSmartPointer<vtkExtractVOI> extractVOI =  vtkSmartPointer<vtkExtractVOI>::New();
    extractVOI->SetInputData(mainSurface);
    extractVOI->SetVOI((int)index[0],(int)index[1],(int)index[2],(int)index[3],(int)index[4],(int)index[5]);
    extractVOI->Update();
    surface=extractVOI->GetOutput();
    cout<<"Extract VOI Done"<<endl;
}

void model::modifyVoxelData(double isoValueStart,double isoValueEnd)
{
	surface=mainSurface;  
	int dims[ 3 ];
	surface->GetDimensions ( dims );
	cout<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<endl;
	cout<<"No. of Scalar Components"<<surface->GetNumberOfScalarComponents()<<endl;

   // double* pPixel = static_cast< double* >( surface->GetScalarPointer () );
    int cn=0;
    int insidecn=0;
    for (int z = 0; z < dims[2]; z++)
    {
      for (int y = 0; y < dims[1]; y++)
      {
        for (int x = 0; x < dims[0]; x++)
        {
          short* pPixel = static_cast<short*>(surface->GetScalarPointer(x,y,z));
          short gValue=pPixel[0];
          if(gValue>2000&&cn<=10)
          {
            cout<<gValue<<endl;
            cn++;
          }  
          if(gValue>=isoValueStart&&gValue<=isoValueEnd)
          {
          	short avg=((isoValueStart+isoValueEnd)/2);
            pPixel[0]=avg;
            insidecn++;
          }
          else
          {
            pPixel[0]=0;
          }
        }
      }
    }
    cout<<"Times it is in the range"<<insidecn<<endl;
}

void model::doMarchingcubes(double isoValueStart,double isoValueEnd)
{
    #if VTK_MAJOR_VERSION <= 5
      marchingCubes->SetInput(surface);
    #else
      marchingCubes->SetInputData(surface);
    #endif
      marchingCubes->ComputeNormalsOff();//computing normals for the marching cubes.
      marchingCubes->ComputeGradientsOff();

  //setting the isovalues to the marching cubes data.
	if(planes->GetNumberOfPlanes()>0)
	{
		marchingCubes->GenerateValues(1, isoValueStart,isoValueEnd);
    }
    else
    {
		marchingCubes->GenerateValues(1, isoValueStart,isoValueEnd);
    }
	marchingCubes->Update();
	cout<<"marchingCubes Done"<<endl;
}

void model::generateSurface(double red,double green,double blue,double opacity,vtkSmartPointer<vtkRenderer> surfacerenderer)
{
	cout<<"Inside Generate Surface Method"<<endl;
  //copy Marching cudes data to polydata.
	polydata=marchingCubes->GetOutput();
	

	cout<<"No.of Planes"<<planes->GetNumberOfPlanes()<<endl;

	vtkSmartPointer<vtkPlaneCollection> plane = vtkSmartPointer<vtkPlaneCollection>::New();
  //clip the surface with the clipping planes where planes are 0,1,2,3,4,5
	if(planes->GetNumberOfPlanes()>0)
	{
    //clipping for plane 0.
		plane->AddItem(planes->GetPlane(0));	
		clipClosedSurface1->SetClippingPlanes(plane);
		clipClosedSurface1->SetInputData(polydata);
		clipClosedSurface1->Update();
		plane->AddItem(planes->GetPlane(1));
		clipClosedSurface2->SetClippingPlanes(plane);
		clipClosedSurface2->SetInputData(clipClosedSurface1->GetOutput());
		clipClosedSurface2->Update();
		plane->AddItem(planes->GetPlane(2));
		clipClosedSurface1->SetClippingPlanes(plane);
		clipClosedSurface1->SetInputData(clipClosedSurface2->GetOutput());
		clipClosedSurface1->Update();
		plane->AddItem(planes->GetPlane(3));
		clipClosedSurface2->SetClippingPlanes(plane);
		clipClosedSurface2->SetInputData(clipClosedSurface1->GetOutput());
		clipClosedSurface2->Update();
		plane->AddItem(planes->GetPlane(4));
		clipClosedSurface1->SetClippingPlanes(plane);
		clipClosedSurface1->SetInputData(clipClosedSurface2->GetOutput());
		clipClosedSurface1->Update();
		plane->AddItem(planes->GetPlane(5));
		clipClosedSurface2->SetClippingPlanes(plane);
		clipClosedSurface2->SetInputData(clipClosedSurface1->GetOutput());
		clipClosedSurface2->Update();
		polydata=clipClosedSurface2->GetOutput();
		//Extract Largest Surface for the polydata
		polydata=extractLS(polydata);
    double* bounds;
    bounds=planes->GetPoints()->GetBounds();
  
  std::cout  << "xmin: " << bounds[0] << " " 
             << "xmax: " << bounds[1] << std::endl
             << "ymin: " << bounds[2] << " " 
             << "ymax: " << bounds[3] << std::endl
             << "zmin: " << bounds[4] << " " 
             << "zmax: " << bounds[5] << std::endl;
    
    double index[6];
    index[0]=(bounds[0]/spacing[0])-origin[0];
    index[1]=(bounds[1]/spacing[0])-origin[0];
    index[2]=(bounds[2]/spacing[1])-origin[1];
    index[3]=(bounds[3]/spacing[1])-origin[1];
    index[4]=(bounds[4]/spacing[2])-origin[2];
    index[5]=(bounds[5]/spacing[2])-origin[2];
    std::cout  << "xmin: " << index[0] << " " 
             << "xmax: " << index[1] << std::endl
             << "ymin: " << index[2] << " " 
             << "ymax: " << index[3] << std::endl
             << "zmin: " << index[4] << " " 
             << "zmax: " << index[5] << std::endl;

    if(index[0]<limits[0])
    {
        index[1]=index[1]+index[0];
        index[0]=0;
    }
    if(index[2]<limits[2])
    {
        index[3]=index[2]+index[3];
        index[2]=0;
    }
    if(index[4]<limits[4])
    {
        index[5]=index[4]+index[5];
        index[4]=0;
    }
    if(index[1]>limits[1])
    {
        index[0]=index[0]-(index[1]-limits[1]);
        index[1]=limits[1];
        index[0]<0?index[0]=limits[0]:index[0];
    }
    if(index[3]>limits[3])
    {
        index[2]=index[2]-(index[3]-limits[3]);
        index[3]=limits[3];
        index[2]<0?index[2]=limits[2]:index[2];
    }
    if(index[5]>limits[5])
    {
        index[4]=index[4]-(index[5]-limits[5]);
        index[5]=limits[5];
        index[4]<0?index[4]=limits[4]:index[4];
    }
    std::cout  << "xmin: " << index[0] << " " 
             << "xmax: " << index[1] << std::endl
             << "ymin: " << index[2] << " " 
             << "ymax: " << index[3] << std::endl
             << "zmin: " << index[4] << " " 
             << "zmax: " << index[5] << std::endl;
    
    cout<<"Before Extract VOI"<<endl;

		
	}


  //assinging the data to mapper then to actor and then it is rendered using renderer.	
	mapper->SetInputData(polydata);
	mapper->Update();
	cout<<"In Surface Before Actor"<<endl;
	vtkSmartPointer<vtkActor> actor =  vtkSmartPointer<vtkActor>::New();
	actor->GetProperty()->SetOpacity(opacity);
	actor->GetProperty()->SetColor(red,green,blue);
	actor->SetMapper(mapper);
	surfacerenderer->AddActor(actor);
	cout<<"In Surface After Actor Creation"<<endl;

	surfacerenderer->SetBackground(.1,.2,.3);
	cout<<"Inside Generate surface Done"<<endl;

}

//extract Large surface from the polydata.
vtkSmartPointer<vtkPolyData> model::extractLS(vtkSmartPointer<vtkPolyData> polydata)
{
  vtkSmartPointer<vtkPolyDataConnectivityFilter> confilter =vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
	confilter->SetInputData(polydata);
	confilter->SetExtractionModeToLargestRegion();
	confilter->Update();
	return confilter->GetOutput();
}

//get the limits from the dicom files.
void model::calculateLimits()
{
    int* dims = dicomReader->GetOutput()->GetDimensions();
    limits[0]=0; limits[1]=dims[0]-1; limits[2]=0; limits[3]=dims[1]-1; limits[4]=0; limits[5]=dims[2]-1;
}

int model::getLimits(int index)
{
    return limits[index];
}


void model::volumeInitialize()
{
  //Intializing Volume from Already Opened Dicom Files
  vtkAlgorithm *reader=0;
  reader=dicomReader;
  reader->Update();

  volmapper->SetInputConnection( reader->GetOutputPort() );
  volmapper->Update();  

  //temp volume to the fast material selection.
  tempvolmapper->SetInputConnection( reader->GetOutputPort() );
  tempvolmapper->Update();  

}

void model::generateVolume(double isoValueStart,double isoValueEnd,double red,double green,double blue,double opacity,bool overwrite,vtkSmartPointer<vtkRenderer> volrenderer) 
{
  // Create our transfer functions
  vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();
  vtkColorTransferFunction *tempcolorFun = vtkColorTransferFunction::New();
  vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();
  vtkPiecewiseFunction *tempopacityFun = vtkPiecewiseFunction::New();

  // Create the property and attach the transfer functions
  vtkVolumeProperty *property = vtkVolumeProperty::New();
  property->SetIndependentComponents(true);
  property->SetColor( colorFun );
  property->SetScalarOpacity( opacityFun );
  property->SetInterpolationTypeToLinear();

  vtkVolumeProperty *tempproperty = vtkVolumeProperty::New();
  tempproperty->SetIndependentComponents(true);
  tempproperty->SetColor( tempcolorFun );
  tempproperty->SetScalarOpacity( tempopacityFun );
  tempproperty->SetInterpolationTypeToLinear();

  cout<<"     Inside Generate Volume Before Setting Color Transfers"<<endl;

  //Assigning colors to the volume R,G,B.
  colorFun->AddRGBPoint( isoValueStart-1, 0.0, 0.0, 0.0 );
  colorFun->AddRGBPoint( isoValueStart, red, green, blue );
  colorFun->AddRGBPoint( isoValueEnd , red, green, blue );
  colorFun->AddRGBPoint( isoValueEnd+1 , 0.0, 0.0, 0.0 );

  tempcolorFun->AddRGBPoint( isoValueStart-1, 0.0, 0.0, 0.0 );
  tempcolorFun->AddRGBPoint( isoValueStart, 1.0, 1.0, 1.0 );
  tempcolorFun->AddRGBPoint( isoValueEnd , 1.0, 1.0, 1.0 );
  tempcolorFun->AddRGBPoint( isoValueEnd+1 , 0.0, 0.0, 0.0 );

  cout<<"     Inside Generate Volume After Setting Color Transfers"<<endl;
  cout<<"     Inside Generate Volume Before Setting Opacity Transfers"<<endl;

  //Assigning opacity to the volume.
  opacityFun->AddPoint(isoValueStart-1, 0 );
  opacityFun->AddPoint(isoValueStart, opacity);
  opacityFun->AddPoint(isoValueEnd, opacity);
  opacityFun->AddPoint(isoValueEnd+1, 0 );

  tempopacityFun->AddPoint(isoValueStart-1, 0 );
  tempopacityFun->AddPoint(isoValueStart, 0.007);
  tempopacityFun->AddPoint(isoValueEnd, 0.007);
  tempopacityFun->AddPoint(isoValueEnd+1, 0 );
  cout<<"     Inside Generate Volume After Setting Opacity Transfers"<<endl;

  volmapper->SetBlendModeToComposite();
  tempvolmapper->SetBlendModeToComposite();

  //setting the lighting for the volume.
  property->ShadeOn();
  property->SetAmbient(0.1);
  property->SetDiffuse(0.9);
  property->SetSpecular(0.2);
  property->SetSpecularPower(10.0);
  property->SetScalarOpacityUnitDistance(0.8919);
  
  tempproperty->ShadeOn();
  tempproperty->SetAmbient(0.1);
  tempproperty->SetDiffuse(0.9);
  tempproperty->SetSpecular(0.2);
  tempproperty->SetSpecularPower(10.0);
  tempproperty->SetScalarOpacityUnitDistance(0.8919);

  // connect up the volume to the property and the mapper
  vtkVolume *volumedata = vtkVolume::New();
  volumedata->SetProperty( property );
  volumedata->SetMapper( volmapper );
  volumedata->Update();

//Temporary volume data and volume mapper for bounding box unclipped part view
	vtkVolume *tempvolumedata = vtkVolume::New();
	tempvolumedata->SetProperty( tempproperty );
	tempvolumedata->SetMapper( tempvolmapper );
	tempvolumedata->Update();

  cout<<"     Inside Generate Volume Settings Done"<<endl;

  if(overwrite)
  {
    cout<<"     Overwrite Option GetSelectedFaceProperty"<<endl;
    volrenderer->RemoveAllViewProps();
  }
  volrenderer->AddViewProp(volumedata);
  volrenderer->AddViewProp(tempvolumedata);
  volrenderer->SetBackground(.0, .0, .0);
  cout<<"     After Adding ViewProp"<<endl;
}

void model::slicesInitialization(int x,int y,int z)
{
  slicesReader=dicomReader;  
  slicesReader->Update();
  //getting the spacing between the spaces of the slices.
  slicesReader->GetOutput()->GetSpacing(spacing);
  cout<<" in slice init  spacing[0]:"<<spacing[0]<<"spacing[1]:"<<spacing[1]<<"spacing[2]:"<<spacing[2]<<endl;

  slicesReader->SetDataExtent(0, x, 0, y, 0, z);
  slicesReader->SetDataSpacing(spacing[0],spacing[1],spacing[2]);
  //setting the slice co oridnates to 0,0,0.
  slicesReader->SetDataOrigin(0.0, 0.0, 0.0);
  slicesReader->SetDataScalarTypeToUnsignedShort();
  slicesReader->SetDataByteOrderToLittleEndian();
  slicesReader->UpdateWholeExtent();
  slicesReader->Update();
}

void model::slicesGeneration(QString sliceName,double isoValueStart,double isoValueEnd,vtkSmartPointer<vtkRenderer> slicesRenderer)
{
   // Calculate the center of the volume
  slicesReader->Update();

  int extent[6];

  slicesReader->GetOutputInformation(0)->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
  slicesReader->GetOutput()->GetSpacing(spacing);
  slicesReader->GetOutput()->GetOrigin(origin);

  double center[3];
  center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
  center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
  center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);
  
  cout<<"origin[0]:"<<origin[0]<<"origin[1]:"<<origin[1]<<"origin[2]:"<<origin[2]<<endl;
  cout<<"spacing[0]:"<<spacing[0]<<"spacing[1]:"<<spacing[1]<<"spacing[2]:"<<spacing[2]<<endl;
  cout<<"extent[0]:"<<extent[0]<<"extent[1]:"<<extent[1]<<"extent[2]:"<<extent[2]<<"extent[3]:"<<extent[3]<<"extent[4]:"<<extent[4]<<"extent[5]:"<<extent[5]<<endl;
  // Matrices for axial, coronal, sagittal, oblique view orientations
  static double axialElements[16] = {
          1, 0, 0, 0,
          0, 1, 0, 0,
          0, 0, 1, 0,
          0, 0, 0, 1 };

  static double coronalElements[16] = {
          1, 0, 0, 0,
          0, 0, 1, 0,
          0,-1, 0, 0,
          0, 0, 0, 1 };

  static double sagittalElements[16] = {
           0, 0,-1, 0,
           1, 0, 0, 0,
           0,-1, 0, 0,
           0, 0, 0, 1 };

  // Set the slice orientation
  vtkSmartPointer<vtkMatrix4x4> resliceAxes =  vtkSmartPointer<vtkMatrix4x4>::New();
    if(sliceName=="axial")
        resliceAxes->DeepCopy(axialElements);
    else if(sliceName=="coronal")
        resliceAxes->DeepCopy(coronalElements);
    else if(sliceName=="sagittal")
        resliceAxes->DeepCopy(sagittalElements);

  // Set the point through which to slice
  resliceAxes->SetElement(0, 3, center[0]);
  resliceAxes->SetElement(1, 3, center[1]);
  resliceAxes->SetElement(2, 3, center[2]);

  // Extract a slice in the desired orientation
  vtkSmartPointer<vtkImageReslice> reslice =  vtkSmartPointer<vtkImageReslice>::New();
  reslice->SetInputConnection(slicesReader->GetOutputPort());
  reslice->SetOutputDimensionality(2);
  reslice->SetResliceAxes(resliceAxes);
  reslice->SetInterpolationModeToLinear();

  vtkSmartPointer<vtkLookupTable> bwLut = vtkSmartPointer<vtkLookupTable>::New();
  bwLut->SetTableRange (isoValueStart, isoValueEnd);
  bwLut->SetSaturationRange (0, 0);
  bwLut->SetHueRange (0, 0);
  bwLut->SetValueRange (0, 1);
  bwLut->Build(); //effective built

  // Map the image through the lookup table
  vtkSmartPointer<vtkImageMapToColors> colors = vtkSmartPointer<vtkImageMapToColors>::New();
  colors->SetInputConnection(reslice->GetOutputPort());
  colors->SetLookupTable(bwLut);
  colors->Update();

  cout<<"Inside Slices Generation Before Actor"<<endl;

  // Display the image
  vtkSmartPointer<vtkImageActor> actor =  vtkSmartPointer<vtkImageActor>::New();
  actor->GetMapper()->SetInputConnection(colors->GetOutputPort());

 // vtkSmartPointer<vtkRenderer> slicesRenderer = vtkSmartPointer<vtkRenderer>::New();
  slicesRenderer->AddActor(actor);

  cout<<"Inside Slices Generation Before Interactor"<<endl;

  //Setup the Interaction
  vtkSmartPointer<vtkInteractorStyleImage> imageStyle = vtkSmartPointer<vtkInteractorStyleImage>::New();
  slicesRenderer->GetRenderWindow()->GetInteractor()->SetInteractorStyle(imageStyle);
  slicesRenderer->SetBackground(.1, .2, .3);

  vtkSmartPointer<vtkImageInteractionCallback> callback = vtkSmartPointer<vtkImageInteractionCallback>::New();
  callback->SetImageReslice(reslice);
  callback->SetInteractor(slicesRenderer->GetRenderWindow()->GetInteractor());

  cout<<"Inside Slices Generation After CallBack"<<endl;

  imageStyle->AddObserver(vtkCommand::MouseMoveEvent, callback);
  imageStyle->AddObserver(vtkCommand::LeftButtonPressEvent, callback);
  imageStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, callback);

}


void model::boxCreation(vtkSmartPointer<vtkRenderer> volrenderer,bool flag)
{
    if(flag==true)
    {
      cout<<"box Creation flag true"<<endl;
      box->SetInteractor(volrenderer->GetRenderWindow()->GetInteractor());
      box->SetPlaceFactor(1.01);
      cout<<"Interactor Set Done"<<endl;
      //setting the clipping planes on surface to locate the clipping planes.
      box->SetInputData(surface);
      box->SetDefaultRenderer(volrenderer);
      box->InsideOutOn();
      box->PlaceWidget();
      cout<<"Renderer Set Done"<<endl;
      vtkBoxWidgetCallback *callback = vtkBoxWidgetCallback::New();
      callback->SetMapper(volmapper);
      box->AddObserver(vtkCommand::InteractionEvent, callback);
      callback->Delete();
      box->EnabledOn();
      box->GetSelectedFaceProperty()->SetOpacity(0.0);
    }
    else
    {
      cout<<"box Creation flag false"<<endl;
      box->SetInteractor(volrenderer->GetRenderWindow()->GetInteractor());
      vtkBoxWidgetCallback *callback = vtkBoxWidgetCallback::New();
      box->AddObserver(vtkCommand::InteractionEvent, callback);
      box->Off();
    }

}

void model::boxDelete(vtkSmartPointer<vtkRenderer> renderer4,vtkSmartPointer<vtkBoxWidget> box)
{

}
    
void model::doReset()
{
    volumeMC->DeepCopy(surface);
    int* dims = volumeMC->GetDimensions();
    limits[0]=0; limits[1]=dims[0]-1; limits[2]=0; limits[3]=dims[1]-1; limits[4]=0; limits[5]=dims[2]-1;
}

//reduse the number of dublicate vertices and select the largest conneted surface.
void model::ApplyDecimationFilter(void)
{
    double reduction_factor;
    vtkSmartPointer<vtkQuadricDecimation> decimation=vtkSmartPointer<vtkQuadricDecimation>::New();
    decimation->SetInputData(poly);
    decimation->SetTargetReduction(0.4);
    decimation->Update();
    poly=decimation->GetOutput();
}

//reduse number of triangles and select the largest conneted surface.
void model::ApplySmoothFilter(void)
{
    vtkSmartPointer<vtkSmoothPolyDataFilter> smoother=vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
    smoother->SetInputData(poly);
    smoother->SetNumberOfIterations(2);
    smoother->SetFeatureAngle(80);
    smoother->SetRelaxationFactor(0.3000);
    smoother->FeatureEdgeSmoothingOn();
    smoother->BoundarySmoothingOn();
    smoother->Update();
    poly=smoother->GetOutput();
}



vtkSmartPointer<vtkActor> model::actorCreation(bool setLargeSurface)
{
    // Creating a Look Up Table for the surface.
    vtkSmartPointer<vtkLookupTable> lut =  vtkSmartPointer<vtkLookupTable>::New();
    lut->SetHueRange(0.66667,0.0);
    lut->SetSaturationRange(1,1);
    lut->SetValueRange(1,1);
    lut->SetAlphaRange(1,1);
    lut->SetNumberOfColors(256);
    lut->Build();


    if(setLargeSurface)
    {
          ApplyDecimationFilter();
          ApplySmoothFilter(); 
          mapper->SetInputData(poly);
          mapper->Update();   
    }
    else
    {
        poly=marchingCubes->GetOutput();
        mapper->SetInputData(poly);
    }
    mapper->ScalarVisibilityOn();
    mapper->SetScalarRange(0, 255);
    mapper->SetLookupTable(lut);

    vtkSmartPointer<vtkActor> actor =  vtkSmartPointer<vtkActor>::New();
    actor->GetProperty()->SetOpacity(1);
    actor->SetMapper(mapper);

    return actor;
}


vtkSmartPointer<vtkSTLWriter> model::saveFile(QString savefile)
{
    vtkSmartPointer<vtkSTLWriter> stlWriter =   vtkSmartPointer<vtkSTLWriter>::New();
    QByteArray ba1 = savefile.toLatin1();
    const char *c_str2 = ba1.data(); 
    stlWriter->SetFileName(c_str2);
    stlWriter->SetFileTypeToBinary();
    stlWriter->SetInputData(polydata);
    stlWriter->Update();
    return stlWriter;
}

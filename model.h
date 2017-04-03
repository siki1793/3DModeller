#ifndef MODEL_H
#define MODEL_H

#include <vtkPolyDataMapper.h>
#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkRenderer.h>
#include <vtkDICOMImageReader.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkLookupTable.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkSTLWriter.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkQuadricDecimation.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractVOI.h>
#include <QMainWindow>
#include <vtkImageReader2.h>
#include <vtkMatrix4x4.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToColors.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorObserver.h>
#include <vtkBoxWidget.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPlanes.h"
#include "vtkPlane.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkAlgorithm.h"
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkActorCollection.h>
#include <vtkClipPolyData.h>
#include <vtkTransform.h>
#include <vtkCutter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleImage.h>
#include <vtkExtractGeometry.h>
#include <vtkDataSet.h>
#include <vtkFillHolesFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkStripper.h>
#include <vtkFeatureEdges.h>
#include <vtkCleanPolyData.h>
#include <vtkAppendPolyData.h>
#include <vtkTriangleFilter.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkMarchingContourFilter.h>
#include <vtkClipClosedSurface.h>
#include <vtkPlaneCollection.h>

class model
{
	public:
		void openFile(QString);
		void volumeInitialize();
		void surfaceInitialize();
		void generateSurface(double ,double ,double ,double ,vtkSmartPointer<vtkRenderer>);
		void modifyVoxelData(double,double);
		void generateVolume(double ,double ,double ,double ,double ,double ,bool ,vtkSmartPointer<vtkRenderer>);
		void calculateLimits();
		int getLimits(int);
		void slicesInitialization(int,int,int);
		void slicesGeneration(QString,double,double,vtkSmartPointer<vtkRenderer>);
		void doMarchingcubes(double,double);
		vtkSmartPointer<vtkSTLWriter> saveFile(QString);
		void extractVOI();
		void doReset();
		vtkSmartPointer<vtkActor> actorCreation(bool);
		vtkSmartPointer<vtkPolyData> extractLS(vtkSmartPointer<vtkPolyData>);
		void ApplyDecimationFilter(void);
		void ApplySmoothFilter(void);
		void boxCreation(vtkSmartPointer<vtkRenderer>,bool );
		void boxDelete(vtkSmartPointer<vtkRenderer> ,vtkSmartPointer<vtkBoxWidget>);
		int limits[6];
};

#endif // MODEL_H

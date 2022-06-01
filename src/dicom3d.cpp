#include "dicom3d.h"
#include <iostream>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkPlane.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVersion.h>
#include <vtkFlyingEdges3D.h>
#include <vtkImageData.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkImageThreshold.h>
#include <vtkAlgorithm.h>
#include <vtkObject.h>
#include <vtkCallbackCommand.h>
#include <vtkClipPolyData.h>
#include <vtkSTLWriter.h>
#include <vtkPolyDataConnectivityFilter.h>
using namespace std;


dicom3d::dicom3d()
{
}

dicom3d::~dicom3d()
{
}

void dicom3d::filterSmallObj(vtkSmartPointer<vtkPolyData> mesh, double ratio, vtkSmartPointer<vtkCallbackCommand> callback)
{
    vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter;

    if (callback.Get() != NULL)
    {
        connectivityFilter->AddObserver(vtkCommand::ProgressEvent, callback);
    }
    connectivityFilter->SetInputData(mesh);
    connectivityFilter->SetExtractionModeToAllRegions();
    connectivityFilter->Update();
    // remove objects consisting of less than ratio vertexes of the biggest object
    vtkIdTypeArray* regionSizes = connectivityFilter->GetRegionSizes();
    long maxSize = 0;
    for (int regions = 0; regions < connectivityFilter->GetNumberOfExtractedRegions(); regions++)
        if (regionSizes->GetValue(regions) > maxSize)
            maxSize = regionSizes->GetValue(regions);

    // append regions of sizes over the threshold
    connectivityFilter->SetExtractionModeToSpecifiedRegions();
    for (int regions = 0; regions < connectivityFilter->GetNumberOfExtractedRegions(); regions++)
        if (regionSizes->GetValue(regions) > maxSize * ratio)
            connectivityFilter->AddSpecifiedRegion(regions);

    connectivityFilter->Update();
    mesh->DeepCopy(connectivityFilter->GetOutput());
}

void dicom3d::clipFunc(vtkSmartPointer<vtkPolyData> mesh, vtkSmartPointer<vtkCallbackCommand> callback)
{
    vtkNew<vtkPlane> clipPlane;
    clipPlane->SetOrigin(mesh->GetCenter());
    clipPlane->SetNormal(0, 0, 1);

    vtkNew<vtkClipPolyData> clipper;
    if (callback.Get() != NULL)
    {
        clipper->AddObserver(vtkCommand::ProgressEvent, callback);
    }
    clipper->SetClipFunction(clipPlane);
    clipper->SetInputData(mesh);
    clipper->Update();
    mesh->DeepCopy(clipper->GetOutput());
}





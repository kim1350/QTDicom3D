#pragma once
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkCallbackCommand.h>
#include <string>
#include <vtkNew.h>
#include <vtkObject.h>
class dicom3d
{
public:
	dicom3d();
	~dicom3d();
	void filterSmallObj(vtkSmartPointer<vtkPolyData> mesh, double ratio, vtkSmartPointer<vtkCallbackCommand> callback);
	void clipFunc(vtkSmartPointer<vtkPolyData> mesh, vtkSmartPointer<vtkCallbackCommand> callback);
private:	
};


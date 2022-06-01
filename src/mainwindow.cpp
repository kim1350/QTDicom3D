#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <iostream>
#include <vtkImageData.h>
#include "vtkDICOMDirectory.h"
#include "vtkDICOMItem.h"
#include "vtkStringArray.h"
#include "vtkDICOMReader.h"
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPlane.h>
#include <vtkClipPolyData.h>
#include <vector>
#include <string>
#include "dicom3d.h"
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
#include <vtkSphereSource.h>
using namespace std;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    //progresser = new FormProgress();
    //connect(this, &MainWindow::signalp, progresser, &FormProgress::slot);
     
    mRenderWindow->AddRenderer(mRenderer);
    mRenderWindow->SetInteractor(mInteractor); 
    //mInteractor->SetInteractorStyle(mInteractorStyle);
    //QObject::connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);

    ui->openGLWidget->setRenderWindow(mRenderWindow);
    mInteractor->Initialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_checkBox_iso_stateChanged(int arg1)
{
    if (ui->checkBox_iso->isChecked()) {
        ui->spinBox_iso_lower->setEnabled(true);
        ui->spinBox_iso_upper->setEnabled(true);
    }
    else {
        ui->spinBox_iso_lower->setEnabled(false);
        ui->spinBox_iso_upper->setEnabled(false);
        upValue = 0;
        lowerValue = 550;
    }
}


void MainWindow::on_checkBox_smooth_stateChanged(int arg1)
{
    if (ui->checkBox_smooth->isChecked()) {
        ui->spinBox_smooth->setEnabled(true);
    }
    else {
        ui->spinBox_smooth->setEnabled(false);
        smoothIter = 0;
    }
}


void MainWindow::on_checkBox_remove_stateChanged(int arg1)
{
    if (ui->checkBox_remove->isChecked()) {
        ui->doubleSpinBox_remove->setEnabled(true);
    }
    else {
        ui->doubleSpinBox_remove->setEnabled(false);
        r = 0.0;
    }
}

 
void MainWindow::on_action_Select_folder_triggered()
{
    QString path_dicom;
    path_dicom = QFileDialog::getExistingDirectory(this, "Choose DICOM directory","/home", QFileDialog::ShowDirsOnly);
    vector <string> vec;
    if (!path_dicom.isEmpty()) {
        vtkNew<vtkDICOMDirectory> dicomDirectory;
        
        dicomDirectory->SetDirectoryName(path_dicom.toStdString().c_str());
        dicomDirectory->SetScanDepth(1);
        dicomDirectory->Update();
        cout << "Read DICOM images located under " << path_dicom.toStdString() << endl;
        cout << "Nbr of patients = " << dicomDirectory->GetNumberOfPatients() << ", ";
        cout << "Nbr of studies = " << dicomDirectory->GetNumberOfStudies() << ", ";
        const int& nbrOfSeries = dicomDirectory->GetNumberOfSeries();
        cout << "Nbr of series = " << nbrOfSeries << endl;
        for (int s = 0; s < nbrOfSeries; s++)
        {
            const vtkDICOMItem& dicomSeries_s = dicomDirectory->GetSeriesRecord(s);
            vtkStringArray* files_s = dicomDirectory->GetFileNamesForSeries(s);
            vtkIdType nbrOfSlices_s = files_s->GetNumberOfValues();
            cout << "(" << s << ")  :  " << nbrOfSlices_s << " files, name = " << dicomSeries_s.Get(DC::SeriesDescription).AsString() << endl;
            string sout = to_string(s) + ' ' + to_string(nbrOfSlices_s)+' '+ dicomSeries_s.Get(DC::SeriesDescription).AsString();
            vec.push_back(sout);
        }


        // choose a particular dicom serie
        
        vtkNew<vtkImageData> Null;
        if (nbrOfSeries > 0) // multiple dicom series
        {
            cout << "Select the dicom series" << endl;
            dialog.addlist(vec);
            dialog.setModal(true);
            dialog.exec();  
            s_nbr = dialog.number_s;
            cout << "selected serie index: " << s_nbr << endl;
        }
        else
        {
            cout << "No DICOM data in directory" << endl;
            QMessageBox::critical(this, "Error", "The algorithm didn't find the .dcm files");
            s_nbr = -1;
        }

        if (s_nbr != -1) {
            this->setCursor(Qt::WaitCursor);
            // load dicom serie
            ui->label->setText(path_dicom);

            const vtkDICOMItem& selected_serie = dicomDirectory->GetSeriesRecord(s_nbr);
            vtkStringArray* files_s = dicomDirectory->GetFileNamesForSeries(s_nbr);
            vtkNew<vtkDICOMReader> reader;
            reader->SetFileNames(dicomDirectory->GetFileNamesForSeries(s_nbr));

            reader->Update();
            volumeR->DeepCopy(reader->GetOutput());
            this->setCursor(Qt::ArrowCursor);
        }
    }

}

void MainWindow::on_action_save_triggered()
{
    QString path_save;
    path_save = QFileDialog::QFileDialog::getSaveFileName(
        this,
        tr("Save stl object"),
        QDir::currentPath(),
        tr("3D object (*.stl)"));
    if (!path_save.isEmpty()) {
        if (polyD) {
            ui->progressBar->setValue(0);
            ui->progressBar->setVisible(true);
            ui->progressBar->setValue(10);
            vtkNew<vtkSTLWriter> exporter;
            if (m_vtkCallback.Get() != NULL)
            {
                exporter->AddObserver(vtkCommand::ProgressEvent, m_vtkCallback);
            }
            exporter->SetInputData(polyD);
            exporter->SetFileName(path_save.toStdString().c_str());       
            exporter->Write();
            ui->progressBar->setValue(100);
            ui->progressBar->setVisible(false);
        }
        else {
            QMessageBox::critical(this, "Error", "Upload the dicom data and generate");
        }
        
    }  

}


void MainWindow::progressCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    // display progress in terminal
    vtkAlgorithm* filter = static_cast<vtkAlgorithm*>(caller);
    if (filter->GetProgress() * 100 == 0) {

        cout << filter->GetClassName() << " progress:" << endl;
    }

    cout << "\r [" << fixed << setprecision(1) << filter->GetProgress() * 100 << "%" << "]";
    if (filter->GetProgress() > 0.999)
        cout << " done!" << endl;
}


void MainWindow::on_pushButton_clicked() {
    if (s_nbr == -1) {
        QMessageBox::critical(this, "Error", "Upload the dicom data");
    }else
    {
        if (ui->checkBox_iso->isChecked()) {
            upValue = ui->spinBox_iso_upper->value();
            lowerValue = ui->spinBox_iso_lower->value();
        }
        if (ui->checkBox_smooth->isChecked()) {
            smoothIter = ui->spinBox_smooth->value();
        }
        if (ui->checkBox_remove->isChecked()) {
            r = ui->doubleSpinBox_remove->value();
        }
        ui->progressBar->setVisible(true);
        ui->progressBar->setValue(10);
        
        m_vtkCallback->SetCallback(progressCallback);
        vtkNew<vtkNamedColors> colors;

        array<unsigned char, 4> skinColor{ {240, 184, 160, 255} };
        colors->SetColor("SkinColor", skinColor.data());
        array<unsigned char, 4> backColor{ {255, 229, 200, 255} };
        colors->SetColor("BackfaceColor", backColor.data());
        array<unsigned char, 4> bkg{ {51, 77, 102, 255} };
        colors->SetColor("BkgColor", bkg.data());

        ui->progressBar->setValue(30);

        mRenderWindow->AddRenderer(mRenderer);
        iren->SetRenderWindow(mRenderWindow);
        //mInteractor->SetRenderWindow(mRenderWindow);

        mRenderWindow->SetInteractor(mInteractor);
        if (upValue != 0 && lowerValue != 0) {
            vtkNew<vtkImageThreshold> imageThreshold;
            imageThreshold->SetInputData(volumeR);
            imageThreshold->ThresholdByUpper(upValue);
            imageThreshold->ReplaceInOn();
            imageThreshold->SetInValue(lowerValue - 1); // mask voxels with a value lower than the lower threshold
            imageThreshold->Update();
            volumeR->DeepCopy(imageThreshold->GetOutput());
        }
        vtkNew<vtkFlyingEdges3D> skinExtractor;
        if (m_vtkCallback.Get() != NULL)
        {
            skinExtractor->AddObserver(vtkCommand::ProgressEvent, m_vtkCallback);
        }

        skinExtractor->SetInputData(volumeR);
        if (lowerValue)
        {
            skinExtractor->SetValue(0, lowerValue);
        }
        skinExtractor->Update();

        ui->progressBar->setValue(50);
        dicom3d d3;

        polyD->DeepCopy(skinExtractor->GetOutput());
        skinExtractor->Delete();
        if (ui->checkBox_cut->isChecked()) {
            d3.clipFunc(polyD, m_vtkCallback); 
        }

        ui->progressBar->setValue(70);

        if (r) {
            d3.filterSmallObj(polyD, r, m_vtkCallback);
        }

        if (smoothIter) {
            vtkNew<vtkSmoothPolyDataFilter> smoother;
            if (m_vtkCallback.Get() != NULL)
            {
                smoother->AddObserver(vtkCommand::ProgressEvent, m_vtkCallback);
            }
            smoother->SetInputData(polyD);
            smoother->SetNumberOfIterations(smoothIter);
            smoother->SetFeatureAngle(45);
            smoother->SetRelaxationFactor(0.05);
            smoother->Update();
            polyD->DeepCopy(smoother->GetOutput());
        }
        vtkNew<vtkPolyDataMapper> skinMapper;
        skinMapper->SetInputData(polyD);
        skinMapper->ScalarVisibilityOff();

        ui->progressBar->setValue(90);
        vtkNew<vtkActor> skin;
        skin->SetMapper(skinMapper);
        skin->GetProperty()->SetDiffuseColor(colors->GetColor3d("SkinColor").GetData());

        vtkNew<vtkProperty> backProp;
        backProp->SetDiffuseColor(colors->GetColor3d("BackfaceColor").GetData());
        skin->SetBackfaceProperty(backProp);

        // An outline provides context around the data.
        //
        vtkNew<vtkOutlineFilter> outlineData;
        outlineData->SetInputData(polyD);

        vtkNew<vtkPolyDataMapper> mapOutline;
        mapOutline->SetInputConnection(outlineData->GetOutputPort());

        vtkNew<vtkActor> outline;
        outline->SetMapper(mapOutline);
        outline->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

        // It is convenient to create an initial view of the data. The FocalPoint
        // and Position form a vector direction. Later on (ResetCamera() method)
        // this vector is used to position the camera to look at the data in
        // this direction.
        vtkNew<vtkCamera> aCamera;
        aCamera->SetViewUp(0, 0, -1);
        aCamera->SetPosition(0, -1, 0);
        aCamera->SetFocalPoint(0, 0, 0);
        aCamera->ComputeViewPlaneNormal();
        aCamera->Azimuth(30.0);
        aCamera->Elevation(30.0);

        // Actors are added to the renderer. An initial camera view is created.
        // The Dolly() method moves the camera towards the FocalPoint,
        // thereby enlarging the image.
        mRenderer->AddActor(outline);
        mRenderer->AddActor(skin);
        mRenderer->SetActiveCamera(aCamera);
        mRenderer->ResetCamera();
        aCamera->Dolly(1.5);

        // Set a background color for the renderer and set the size of the
        // render window (expressed in pixels).
        mRenderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
        mRenderer->ResetCameraClippingRange();
        mRenderWindow->Render();
        ui->progressBar->setValue(100);
        ui->progressBar->setVisible(false); 
        iren->Initialize();
        iren->Start();
    }
    
}


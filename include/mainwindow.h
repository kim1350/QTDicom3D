#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <QVTKInteractor.h>
#include <vtkInteractorStyle.h>
#include "dialog.h"
#include "formprogress.h"

namespace Ui {
class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_checkBox_iso_stateChanged(int arg1);
    void on_checkBox_smooth_stateChanged(int arg1);
    void on_checkBox_remove_stateChanged(int arg1);
    void on_action_Select_folder_triggered();
    void on_action_save_triggered();
 
private:
    Ui::MainWindow *ui;
    vtkNew<vtkGenericOpenGLRenderWindow> mRenderWindow;
    vtkNew<vtkRenderer> mRenderer;
    vtkNew<QVTKInteractor> mInteractor;
    vtkNew<vtkInteractorStyle> mInteractorStyle;
    vtkNew<vtkRenderWindowInteractor> iren;
    static void progressCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    Dialog dialog;
    vtkNew<vtkCallbackCommand> m_vtkCallback;
    FormProgress *progresser;
    vtkNew<vtkImageData> volumeR;
    vtkNew<vtkPolyData> polyD;
    int s_nbr=-1;
    int upValue = 0, lowerValue = 550, smoothIter = 0;
    float r = 0.0;
signals:
   void signalp(int);
};


#endif // MAINWINDOW_H

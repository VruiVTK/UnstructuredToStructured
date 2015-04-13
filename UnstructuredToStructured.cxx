#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkExodusIIReader.h>
#include <vtkNew.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkActor.h>
#include <vtkVolume.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkProperty.h>
#include <vtkImageData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkCellDataToPointData.h>
#include <vtkDoubleArray.h>
#include <vtkTimerLog.h>

#include <iostream>
#include <sstream>

#define CHECKERBOARD 0

#if CHECKERBOARD
#include <vtkCheckerboardSplatter.h>
#else
#include <vtkGaussianSplatter.h>
#endif

int main(int argc, char* argv[])
{
//  vtkNew<vtkRenderWindow> renWin;
//  vtkNew<vtkRenderer> ren;
//  renWin->AddRenderer(ren.GetPointer());
//  vtkNew<vtkRenderWindowInteractor> iren;
//  iren->SetRenderWindow(renWin.GetPointer());
//  vtkNew<vtkInteractorStyleTrackballCamera> style;
//  iren->SetInteractorStyle(style.GetPointer());

  vtkNew<vtkExodusIIReader> reader;
  reader->SetFileName(argv[1]);
  reader->UpdateInformation();
  reader->GenerateObjectIdCellArrayOff();
  reader->GenerateGlobalElementIdArrayOff();
  reader->GenerateGlobalNodeIdArrayOff();

  int arrayCount = 0;
  for (int i = 0; i < reader->GetNumberOfPointResultArrays(); ++i)
    {
    std::string arrayName(reader->GetPointResultArrayName(i));
    std::string gr("gr");
    if (arrayName.compare(0, 2, gr) == 0)
      {
      reader->SetPointResultArrayStatus(arrayName.c_str(), 1);
      arrayCount++;
      }
    }
//  for (int i = 0; i < reader->GetNumberOfElementResultArrays(); ++i)
//    {
//    reader->SetElementResultArrayStatus(reader->GetElementResultArrayName(i), 1);
//    }


  reader->Update();

  vtkNew<vtkTimerLog> timer;
  std::cout << "Start timer" << std::endl;
  timer->StartTimer();

  double dif = 256.0/arrayCount;

  vtkSmartPointer<vtkMultiBlockDataSet> mb = reader->GetOutput();

//  for (vtkIdType i = 0; i < mb->GetNumberOfBlocks(); ++i)
  for (vtkIdType i = 0; i < 1; ++i)
    {
    vtkSmartPointer<vtkMultiBlockDataSet> mbi = vtkMultiBlockDataSet::SafeDownCast(mb->GetBlock(i));
    if (mbi)
      {
//      for (vtkIdType j = 0; j < mbi->GetNumberOfBlocks(); ++j)
      //We know that all the information is in the first block
      for (vtkIdType j = 0; j < 1; ++j)
        {
        vtkSmartPointer<vtkUnstructuredGrid> usg = vtkUnstructuredGrid::SafeDownCast(mbi->GetBlock(j));
        if (usg)
          {
//          vtkNew<vtkCellDataToPointData> cellToPoint;
//          cellToPoint->SetInputData(usg);
//          cellToPoint->Update();
//          vtkSmartPointer<vtkUnstructuredGrid> usgg = vtkUnstructuredGrid::SafeDownCast(
//            cellToPoint->GetOutput());

          vtkSmartPointer<vtkImageData> imageData =
            vtkSmartPointer<vtkImageData>::New();
          vtkSmartPointer<vtkDoubleArray> imageArray =
            vtkSmartPointer<vtkDoubleArray>::New();
          imageArray->SetName("GrainSegmentationValues");
          imageArray->SetNumberOfComponents(1);
          for (vtkIdType k = 0; k < arrayCount; ++k)
            {
            usg->GetPointData()->SetActiveScalars(usg->GetPointData()->GetArrayName(k));
            std::cout << "Splatting array: " <<
              usg->GetPointData()->GetArrayName(k) << std::endl;

#if CHECKERBOARD
            vtkNew<vtkCheckerboardSplatter> gaussian;
#else
            vtkNew<vtkGaussianSplatter> gaussian;
#endif
            gaussian->SetInputData(usg);
            gaussian->SetModelBounds(usg->GetBounds());
            gaussian->ScalarWarpingOn();
            gaussian->NormalWarpingOff();
            gaussian->Update();
            vtkSmartPointer<vtkImageData> im = gaussian->GetOutput();
            vtkSmartPointer<vtkDataArray> dataArray =
                im->GetPointData()->GetScalars();
            double * dataRange = dataArray->GetRange();
            if (k == 0)
              {
              imageData->DeepCopy(im);
              imageArray->SetNumberOfTuples(dataArray->GetNumberOfTuples());
              imageData->GetPointData()->AddArray(imageArray);
              imageArray->FillComponent(0, 0.0);
              }

            for (vtkIdType val = 0; val < dataArray->GetNumberOfTuples(); ++val)
              {
              if (dataArray->GetTuple1(val) > dataRange[1]*0.8)
                {
                imageArray->SetTuple1(val, k+1);
//                imageArray->SetTuple1(val, (double)k*(255-dif)/arrayCount + dif);
                }
              }
          }

          imageData->GetPointData()->SetActiveScalars("GrainSegmentationValues");
          vtkNew<vtkXMLImageDataWriter> writer1;
          writer1->SetInputData(imageData);
          std::stringstream ss1;
#if CHECKERBOARD
          ss1 << "imageData_checkerboard" << "_" << i << "_" << j << ".vti";
#else
          ss1 << "imageData_gaussian" << "_" << i << "_" << j << ".vti";
#endif
          writer1->SetFileName(ss1.str().c_str());
          std::cout << "Writing out " << ss1.str().c_str() << std::endl;
          writer1->Write();
          writer1->Update();
          timer->StopTimer();
          std::cout << "Stop Timer. Time spent = " << timer->GetElapsedTime() << std::endl;
          }
        }
      }
    }
  return EXIT_SUCCESS;
}

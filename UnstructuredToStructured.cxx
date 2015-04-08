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
#include <vtkGaussianSplatter.h>
#include <vtkCellDataToPointData.h>
#include <vtkDoubleArray.h>
#include <vtkProjectedTetrahedraMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkDataSetTriangleFilter.h>

#include <iostream>
#include <sstream>

int main(int argc, char* argv[])
{
  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderer> ren;
  renWin->AddRenderer(ren.GetPointer());
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin.GetPointer());
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  iren->SetInteractorStyle(style.GetPointer());

  vtkNew<vtkExodusIIReader> reader;
  reader->SetFileName(argv[1]);
  reader->UpdateInformation();
  reader->GenerateObjectIdCellArrayOff();
  reader->GenerateGlobalElementIdArrayOff();
  reader->GenerateGlobalNodeIdArrayOff();
//  reader->SetTimeStep(14);

  int arrayCount = 0;
  for (int i = 0; i < reader->GetNumberOfPointResultArrays(); ++i)
    {
    std::string arrayName(reader->GetPointResultArrayName(i));
//    std::string gr("gr");
//    if (arrayName.compare(0, 2, gr) == 0)
//      {
      reader->SetPointResultArrayStatus(arrayName.c_str(), 1);
      arrayCount++;
//      }
    }
//  for (int i = 0; i < reader->GetNumberOfElementResultArrays(); ++i)
//    {
//    reader->SetElementResultArrayStatus(reader->GetElementResultArrayName(i), 1);
//    }


  reader->Update();



  double dif = 256.0/arrayCount;

  vtkSmartPointer<vtkMultiBlockDataSet> mb = reader->GetOutput();

  for (vtkIdType i = 0; i < mb->GetNumberOfBlocks(); ++i)
    {
    vtkSmartPointer<vtkMultiBlockDataSet> mbi = vtkMultiBlockDataSet::SafeDownCast(mb->GetBlock(i));
    if (mbi)
      {
//      for (vtkIdType j = 0; j < mbi->GetNumberOfBlocks(); ++j)
      for (vtkIdType j = 0; j < 1; ++j)
        {
        vtkSmartPointer<vtkUnstructuredGrid> usg = vtkUnstructuredGrid::SafeDownCast(mbi->GetBlock(j));
        if (usg)
          {
          vtkNew<vtkDataSetTriangleFilter> tf;
          tf->SetInputData(usg);
          tf->Update();
          tf->GetOutput()->GetPointData()->SetActiveScalars("Pres");
          vtkNew<vtkProjectedTetrahedraMapper> mapper;
          mapper->SetInputConnection(tf->GetOutputPort());
          mapper->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_POINTS, "Pres");
          vtkNew<vtkVolume> volume;
          volume->SetMapper(mapper.GetPointer());
          vtkNew<vtkRenderer> ren;
          ren->AddVolume(volume.GetPointer());
          ren->UseDepthPeelingOn();
          ren->SetOcclusionRatio(0.4);
          vtkNew<vtkRenderWindow> renWin;
          renWin->SetAlphaBitPlanes(1);
          renWin->AddRenderer(ren.GetPointer());
          vtkNew<vtkVolumeProperty> vp;
          vtkNew<vtkColorTransferFunction> cf;
          cf->AddRGBPoint(0.003, 255, 0, 0);
          cf->AddRGBPoint(0.02, 0,0,255);
          cf->Build();
          vp->SetColor(cf.GetPointer());
          vtkNew<vtkPiecewiseFunction> pw;
          pw->AddPoint(0.001, 0.0);
          pw->AddPoint(0.03, 1.0);
          vp->SetScalarOpacity(pw.GetPointer());
          volume->SetProperty(vp.GetPointer());
          vtkNew<vtkRenderWindowInteractor> iren;
          iren->SetRenderWindow(renWin.GetPointer());
          ren->ResetCamera();
          iren->Initialize();
          iren->Start();
//          renWin->Render();
//          vtkNew<vtkCellDataToPointData> cellToPoint;
//          cellToPoint->SetInputData(usg);
//          cellToPoint->Update();
//          vtkSmartPointer<vtkUnstructuredGrid> usgg = vtkUnstructuredGrid::SafeDownCast(
//            cellToPoint->GetOutput());
//
//          vtkNew<vtkDataSetTriangleFilter> triangle;
//          triangle->SetInputData(usg);
//          triangle->Update();

//          vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
//          vtkSmartPointer<vtkDoubleArray> imageArray =
//            vtkSmartPointer<vtkDoubleArray>::New();
//          imageArray->SetName("GrainSegmentationValues");
//          imageArray->SetNumberOfComponents(1);
//          for (vtkIdType k = 0; k < arrayCount; ++k)
//            {
//            usg->GetPointData()->SetActiveScalars(usg->GetPointData()->GetArrayName(k));
//            std::cout << usg->GetPointData()->GetArrayName(k) << std::endl;
//
//            vtkNew<vtkGaussianSplatter> gaussian;
//            gaussian->SetInputData(usg);
//            gaussian->SetModelBounds(usg->GetBounds());
//            gaussian->ScalarWarpingOn();
//            gaussian->NormalWarpingOff();
//            gaussian->SetRadius(0.01);
//            gaussian->SetExponentFactor(-1);
//            gaussian->Update();
//            vtkSmartPointer<vtkImageData> im = vtkImageData::SafeDownCast(gaussian->GetOutput());
//            vtkSmartPointer<vtkDataArray> dataArray = vtkDataArray::SafeDownCast(im->GetPointData()->GetArray("SplatterValues"));
//            double * dataRange = dataArray->GetRange();
//            if (k == 0)
//              {
//              imageData->DeepCopy(im);
//              imageArray->SetNumberOfTuples(dataArray->GetNumberOfTuples());
//              imageData->GetPointData()->AddArray(imageArray);
//              imageArray->FillComponent(0, 0.0);
//              }
//
//            for (vtkIdType val = 0; val < dataArray->GetNumberOfTuples(); ++val)
//              {
//              if (dataArray->GetTuple1(val) > dataRange[1]*0.8)
//                {
//                imageArray->SetTuple1(val, k+1);
////                imageArray->SetTuple1(val, (double)k*(255-dif)/arrayCount + dif);
//                }
//              }
//            }
////          vtkNew<vtkXMLImageDataWriter> writer1;
////          writer1->SetInputData(imageData);
////          std::stringstream ss1;
////          ss1 << argv[1] << "_" << i << "_" << j << ".vti";
////          writer1->SetFileName(ss1.str().c_str());
////          writer1->Write();
////          writer1->Update();
          }
        }
      }
    }
  return EXIT_SUCCESS;
}

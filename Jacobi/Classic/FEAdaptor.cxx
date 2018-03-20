#include <iostream>
#include "FEAdaptor.h"

#include <vtkCPDataDescription.h>
#include <vtkCPInputDataDescription.h>
#include <vtkCPProcessor.h>
#include <vtkCPPythonScriptPipeline.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkImageData.h>

namespace
{
  vtkCPProcessor* Processor = NULL;
  vtkImageData* VTKGrid;

  void BuildVTKGrid(unsigned int ext_minX, unsigned int ext_maxX,
                    unsigned int ext_minY, unsigned int ext_maxY,
                    unsigned int ext_minZ, unsigned int ext_maxZ)
  {
  VTKGrid->SetExtent(ext_minX, ext_maxX,
                     ext_minY, ext_maxY,
                     ext_minZ, ext_maxZ);
  }

  void UpdateVTKAttributes(unsigned int numberOfPoints, double* temperatureData)
  {
    if(VTKGrid->GetPointData()->GetNumberOfArrays() == 0)
      {
      // temperature array
      vtkNew<vtkDoubleArray> temperature;
      temperature->SetName("temperature");
      temperature->SetNumberOfComponents(1);
      VTKGrid->GetPointData()->AddArray(temperature.GetPointer());
      //fprintf(stdout,"temperature->AddArray(temperatureData)\n");
      }

    vtkDoubleArray* temperature = vtkDoubleArray::SafeDownCast(
      VTKGrid->GetPointData()->GetArray("temperature"));
    // The temperature array is a scalar array so we can reuse
    // memory as long as we ordered the points properly.
    temperature->SetArray(temperatureData, static_cast<vtkIdType>(numberOfPoints), 1);
    //fprintf(stdout,"  temperature->SetArray(temperatureData)\n");
  }

  void BuildVTKDataStructures(unsigned int x0, unsigned int x1,
                              unsigned int y0, unsigned int y1,
                              unsigned int z0, unsigned int z1, double* temperature)
  {
    if(VTKGrid == NULL)
      {
      // The grid structure isn't changing so we only build it
      // the first time it's needed. We pass the extents
      VTKGrid = vtkImageData::New();
      BuildVTKGrid(x0, x1, y0, y1, z0, z1);
      }
    UpdateVTKAttributes((x1-x0+1) * (y1-y0+1) * (z1-z0+1), temperature);
  }
}

void CatalystInitialize(int numScripts, char* scripts[])
{
  if(Processor == NULL)
    {
    Processor = vtkCPProcessor::New();
    Processor->Initialize();
    }
  else
    {
    Processor->RemoveAllPipelines();
    }
  for(int i=1;i<numScripts;i++)
    {
    vtkNew<vtkCPPythonScriptPipeline> pipeline;
    pipeline->Initialize(scripts[i]);
    Processor->AddPipeline(pipeline.GetPointer());
    }
}

void CatalystFinalize()
{
  if(Processor)
    {
    Processor->Delete();
    Processor = NULL;
    }
  if(VTKGrid)
    {
    VTKGrid->Delete();
    VTKGrid = NULL;
    }
}

void CatalystCoProcess(unsigned int m, unsigned int x0, unsigned int x1,
                       unsigned int y0, unsigned int y1,
                       unsigned int z0, unsigned int z1,
                       double* temperatureData, double time,
                       unsigned int timeStep, int lastTimeStep)
{
  vtkNew<vtkCPDataDescription> dataDescription;
  dataDescription->AddInput("input");
  //fprintf(stdout,"CatalystCoProcess(%d, %f)\n", timeStep, time );
  dataDescription->SetTimeData(time, timeStep);
  if(lastTimeStep == true)
    {
    // assume that we want to all the pipelines to execute if it
    // is the last time step.
    dataDescription->ForceOutputOn();
    //fprintf(stdout,"  lastTimeStep == true, ForceOutputOn\n");
    }
  if(Processor->RequestDataDescription(dataDescription.GetPointer()) != 0)
    {
    BuildVTKDataStructures(x0, x1, y0, y1, z0, z1, temperatureData);
    dataDescription->GetInputDescriptionByName("input")->SetGrid(VTKGrid);
    int wholeExtent[6];
    wholeExtent[0] = wholeExtent[2] = wholeExtent[4] = wholeExtent[5] = 0;
    wholeExtent[1] = wholeExtent[3] = m+1;
    dataDescription->GetInputDescriptionByName("input")->SetWholeExtent(wholeExtent);
    Processor->CoProcess(dataDescription.GetPointer());
    //fprintf(stdout,"  Processor->CoProcess\n");
    }
}

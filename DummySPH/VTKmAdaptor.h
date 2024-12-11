#ifndef VTKmAdaptor_h
#define VTKmAdaptor_h

#include <vtkm/cont/ColorTable.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DataSetBuilderExplicit.h>
#include <vtkm/cont/ArrayHandleExtractComponent.h>
#include <vtkm/cont/ArrayHandleStride.h>
#include <vtkm/cont/Initialize.h>
#include <vtkm/io/VTKDataSetWriter.h>
#include <vtkm/rendering/Actor.h>
#include <vtkm/rendering/CanvasRayTracer.h>

#include <vtkm/rendering/MapperPoint.h>
#include <vtkm/rendering/MapperRayTracer.h>
#include <vtkm/rendering/MapperWireframer.h>
#include <vtkm/rendering/MapperGlyphScalar.h>

#include <vtkm/rendering/Scene.h>
#include <vtkm/rendering/View3D.h>

namespace VTKmAdaptor
{
  vtkm::rendering::CanvasRayTracer canvas(1080, 1080);
  vtkm::rendering::Scene           scene;
  vtkm::rendering::MapperPoint       mapper0; // rendering crash
  vtkm::rendering::MapperRayTracer   mapper1; // no rendering erros but empty output 
  vtkm::rendering::MapperWireframer  mapper2; // rendering errors out, creates empty images
  vtkm::rendering::MapperGlyphScalar mapper3; // rendering crash
  vtkm::cont::DataSet              dataSet;
  std::vector<vtkm::Id>            connectivity;

template<typename T>
void Initialize(int argc, char* argv[], sph::ParticlesData<T> *sim)
{
  std::cout << "VTK-m::Initialize" << std::endl;
  vtkm::cont::Initialize(argc, argv);

  vtkm::cont::DataSetBuilderExplicit dataSetBuilder;

  connectivity.resize(sim->n); 
  std::iota(connectivity.begin(), connectivity.end(), 0);
  
  vtkm::cont::ArrayHandle<vtkm::Vec3f> coordsArray;
  coordsArray.Allocate(static_cast<vtkm::Id>(sim->n));
  auto coordsPortal = coordsArray.WritePortal();
  for (std::size_t index = 0; index < sim->n; ++index)
  {
    coordsPortal.Set(static_cast<vtkm::Id>(index),
                     vtkm::make_Vec(static_cast<vtkm::FloatDefault>(sim->x[index]),
                                    static_cast<vtkm::FloatDefault>(sim->y[index]),
                                    static_cast<vtkm::FloatDefault>(sim->z[index])));
  }
  auto connArray = vtkm::cont::make_ArrayHandle(connectivity, vtkm::CopyFlag::Off);
  vtkm::IdComponent numberOfPointsPerCell = 1;
  dataSet = dataSetBuilder.Create(coordsArray, // use template line 230
                                                      vtkm::CellShapeTagVertex(),
                                                      numberOfPointsPerCell,
                                                      connArray, "coords");
 /**/
 /*
   vtkm::cont::DataSet dataSet = dataSetBuilder.Create(coordsArray,
                                                      shapes,
                                                      numIndices,
                                                      connectivity);
   */
   /*
  vtkm::cont::DataSet dataSet = dataSetBuilder.Create(sim->x, // use template line 260
                                                      sim->y,
                                                      sim->z,
                                                      shapes,
                                                      numIndices,
                                                      connectivity);
  */

#ifdef STRIDED_SCALARS
  // NOTE: In this case, the num_vals, needs to be
  // the full extent of the strided area, thus sim->n*sim->NbofScalarfields
  std::cout << "creating fields with strided access\n";
  auto ScalarsArray = vtkm::cont::make_ArrayHandle<T>(sim->scalars.data(), sim->n*sim->NbofScalarfields, vtkm::CopyFlag::Off);
  vtkm::cont::ArrayHandleStride<T> dataArray1(ScalarsArray, sim->n, 3, 0);
  dataSet.AddPointField("Density", dataArray1);
  
  vtkm::cont::ArrayHandleStride<T> dataArray2(ScalarsArray, sim->n, 3, 1);
  dataSet.AddPointField("Pressure", dataArray2);
  
  vtkm::cont::ArrayHandleStride<T> dataArray3(ScalarsArray, sim->n, 3, 2);
  dataSet.AddPointField("cst-field", dataArray3);
#else
  std::cout << "creating fields with independent (stride=1) access\n";
//https://vtk-m.readthedocs.io/en/stable/basic-array-handles.html#ex-arrayhandlefromvector
  auto dataArray1 = vtkm::cont::make_ArrayHandle(sim->scalar1, vtkm::CopyFlag::Off);
  dataSet.AddPointField("Density", dataArray1);
  
  auto dataArray2 = vtkm::cont::make_ArrayHandle(sim->scalar2, vtkm::CopyFlag::Off);
  dataSet.AddPointField("Pressure", dataArray2);
  
  auto dataArray3 = vtkm::cont::make_ArrayHandle(sim->scalar3, vtkm::CopyFlag::Off);
  dataSet.AddPointField("cst-field", dataArray3);
#endif

  dataSet.PrintSummary(std::cout);
  

  
    //Creating Actor
  vtkm::cont::ColorTable colorTable("viridis");
  vtkm::rendering::Actor actor(dataSet.GetCellSet(),
                               dataSet.GetCoordinateSystem(),
                               dataSet.GetField("Pressure"),
                               colorTable);

  // Adding Actor to the scene
  scene.AddActor(actor);
/*
  mapper0.SetUsePoints();
  mapper0.SetRadius(0.02f);
  mapper0.UseVariableRadius(false);
  mapper0.SetRadiusDelta(0.05f);
  */
}

void Execute(int it, int frequency)
{

  std::ostringstream fname;
  if(it % frequency == 0)
    {
    fname << "insitu." << it << ".png";
    vtkm::rendering::View3D view(scene, mapper3, canvas);
    view.SetBackgroundColor(vtkm::rendering::Color(1.0f, 1.0f, 1.0f));
    view.SetForegroundColor(vtkm::rendering::Color(0.0f, 0.0f, 0.0f));
    view.Paint();
    view.SaveAs(fname.str());
    }
}

void Finalize()
{
  vtkm::io::VTKDataSetWriter writer("/dev/shm/dummy.vtk");
  writer.SetFileTypeToBinary();
  writer.WriteDataSet(dataSet);
}
}
#endif


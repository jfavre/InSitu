#ifndef VTKmAdaptor_h
#define VTKmAdaptor_h

#include <vtkm/cont/ColorTable.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DataSetBuilderExplicit.h>
#include <vtkm/cont/Initialize.h>
#include <vtkm/io/VTKDataSetWriter.h>
#include <vtkm/rendering/Actor.h>
#include <vtkm/rendering/CanvasRayTracer.h>
#include <vtkm/rendering/MapperPoint.h>
#include <vtkm/rendering/Scene.h>
#include <vtkm/rendering/View3D.h>

namespace VTKmAdaptor
{
  vtkm::rendering::CanvasRayTracer canvas(1080, 1080);
  vtkm::rendering::Scene           scene;
  vtkm::rendering::MapperPoint     mapper;

  vtkm::cont::DataSet              dataSet;
  std::vector<vtkm::Id>            connectivity;

template<typename T>
void Initialize(int argc, char* argv[], sph::ParticlesData<T> *sim)
{
  std::cout << "VTK-m::Initialize" << std::endl;
  vtkm::cont::Initialize(argc, argv);

  vtkm::cont::DataSetBuilderExplicit dataSetBuilder;
  /*
  const std::vector<vtkm::UInt8> shapes(sim->n, vtkm::CELL_SHAPE_VERTEX);
  const std::vector<vtkm::IdComponent> numIndices(sim->n, 1);
  */
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
  dataSet = dataSetBuilder.Create(coordsArray, // use template line 230
                                                      vtkm::CellShapeTagVertex(),
                                                      static_cast<vtkm::IdComponent>(1),
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

  auto dataArray = vtkm::cont::make_ArrayHandle(sim->scalar1, vtkm::CopyFlag::Off);
  dataSet.AddPointField("Density", dataArray);

    //Creating Actor
  vtkm::cont::ColorTable colorTable("viridis");
  vtkm::rendering::Actor actor(dataSet.GetCellSet(),
                               dataSet.GetCoordinateSystem(),
                               dataSet.GetField("Density"),
                               colorTable);

  // Adding Actor to the scene
  scene.AddActor(actor);

  mapper.SetUsePoints();
  mapper.SetRadius(0.02f);
  mapper.UseVariableRadius(true);
  mapper.SetRadiusDelta(0.05f);
}

void Execute(int it, int frequency)
{
  std::ostringstream fname;
  if(it % frequency == 0)
    {
    fname << "insitu." << it << ".png";
    vtkm::rendering::View3D view(scene, mapper, canvas);
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


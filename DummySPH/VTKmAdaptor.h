#ifndef VTKmAdaptor_h
#define VTKmAdaptor_h

#include <iomanip>

#include <vtkm/cont/ColorTable.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/ArrayCopy.h>
#include <vtkm/cont/ArrayCopyDevice.h>
#include <vtkm/cont/DataSetBuilderExplicit.h>
#include <vtkm/cont/ArrayHandleExtractComponent.h>
#include <vtkm/cont/ArrayHandleCompositeVector.h>
#include <vtkm/cont/ArrayHandleStride.h>
#include <vtkm/cont/ArrayHandleIndex.h>
#include <vtkm/cont/Initialize.h>
#include <vtkm/io/VTKDataSetWriter.h>
#include <vtkm/rendering/Actor.h>
#include <vtkm/rendering/CanvasRayTracer.h>

#include <vtkm/rendering/MapperPoint.h>
#include <vtkm/rendering/MapperRayTracer.h>
#include <vtkm/rendering/MapperWireframer.h>
#include <vtkm/rendering/MapperGlyphScalar.h>
#include <vtkm/filter/resampling/HistSampling.h>
#include <vtkm/filter/geometry_refinement/ConvertToPointCloud.h>
#include <vtkm/filter/entity_extraction/ThresholdPoints.h>
#include <vtkm/rendering/Scene.h>
#include <vtkm/rendering/View3D.h>
/****************************************

three methods have been identified to create coordinates array handles from the
given std::vector<T> x, y, z;         // Positions

1) vtkm::cont::ArrayHandle<vtkm::Vec3f> coordsArray0;
   coordsArray0.Allocate(static_cast<vtkm::Id>(sim->n));

2) coordsArray1 = vtkm::cont::make_ArrayHandleSOA<vtkm::Vec3f>({sim->x, sim->y, sim->z});;

3) coordsArray2 = vtkm::cont::make_ArrayHandleCompositeVector(component1, component2, component3);

****************************************/
namespace VTKmAdaptor
{
  vtkm::rendering::CanvasRayTracer   canvas(1024,1024);
  vtkm::rendering::Scene             scene;
  vtkm::rendering::MapperPoint       mapper0; // rendering crash
  vtkm::rendering::MapperRayTracer   mapper1; // no rendering errors but empty output 
  vtkm::rendering::MapperWireframer  mapper2; // rendering errors out, creates empty images
  vtkm::rendering::MapperGlyphScalar mapper3; // rendering crash
  vtkm::cont::DataSet                dataSet;
  vtkm::Bounds bounds;

template<typename T>
void Initialize(int argc, char* argv[], sph::ParticlesData<T> *sim)
{
  std::cout << "VTK-m::Initialize" << std::endl;

  vtkm::cont::Initialize(argc, argv);

  vtkm::cont::DataSetBuilderExplicit dataSetBuilder;
  /*
  vtkm::cont::ArrayHandle<vtkm::Vec3f> coordsArray0;
  coordsArray0.Allocate(static_cast<vtkm::Id>(sim->n));

  auto coordsPortal = coordsArray0.WritePortal();
  for (std::size_t index = 0; index < sim->n; ++index)
  {
    coordsPortal.Set(static_cast<vtkm::Id>(index),
                     vtkm::make_Vec(static_cast<vtkm::FloatDefault>(sim->x[index]),
                                    static_cast<vtkm::FloatDefault>(sim->y[index]),
                                    static_cast<vtkm::FloatDefault>(sim->z[index])));
  }
  std::cout << "COORDARRAY0 HANDLE" << std::endl;
  vtkm::cont::printSummary_ArrayHandle(coordsArray0, std::cout);
  std::cout << "--------------------------" << std::endl;
  */
  /****************************************/
#ifdef STRIDED_SCALARS
  auto AOS = vtkm::cont::make_ArrayHandle<T>(&sim->scalarsAOS[0].mass,
                                             sim->n * sim->NbofScalarfields,
                                             vtkm::CopyFlag::Off);

  vtkm::cont::ArrayHandleStride<T> pos_x (AOS, sim->n, sim->NbofScalarfields, 1);
  vtkm::cont::ArrayHandleStride<T> pos_y (AOS, sim->n, sim->NbofScalarfields, 2);
  vtkm::cont::ArrayHandleStride<T> pos_z (AOS, sim->n, sim->NbofScalarfields, 3);
#else
/*
  auto coordsArray1 =
    vtkm::cont::make_ArrayHandleSOA<vtkm::Vec3f>({sim->x, sim->y, sim->z});
    
  vtkm::cont::printSummary_ArrayHandle(coordsArray1, std::cout);
  vtkm::cont::ArrayHandle<vtkm::Vec3f> positions1;
  vtkm::cont::ArrayCopyDevice(coordsArray1, positions1);
  std::cout << "COORDARRAY1 HANDLE" << std::endl;
  vtkm::cont::printSummary_ArrayHandle(coordsArray1, std::cout);
  std::cout << "--------------------------" << std::endl;
  */
  /****************************************/
  // https://vtk-m.readthedocs.io/en/v2.2.0/fancy-array-handles.html#composite-vector-arrays
  auto pos_x = vtkm::cont::make_ArrayHandle<T>(sim->x, vtkm::CopyFlag::Off);
  auto pos_y = vtkm::cont::make_ArrayHandle<T>(sim->y, vtkm::CopyFlag::Off);
  auto pos_z = vtkm::cont::make_ArrayHandle<T>(sim->z, vtkm::CopyFlag::Off);
#endif
  
  auto coordsArray2 =
    vtkm::cont::make_ArrayHandleCompositeVector(pos_x, pos_y, pos_z);
  //std::cout << "COORDARRAY2 HANDLE" << std::endl;
  //vtkm::cont::printSummary_ArrayHandle(coordsArray2, std::cout);
  //std::cout << "--------------------------" << std::endl;
  
  vtkm::cont::ArrayHandle<vtkm::Vec3f> positions2;
  vtkm::cont::ArrayCopy(coordsArray2, positions2);
  //std::cout << "POSITIONS2 HANDLE" << std::endl;
  //vtkm::cont::printSummary_ArrayHandle(positions2, std::cout);
  //std::cout << "--------------------------" << std::endl;
  
  bounds = vtkm::Bounds(vtkm::Vec3f_64(-1.0, -1.0, -1.0),
                        vtkm::Vec3f_64(2.0*sim->par_size - 1.0,
                                       2.0*sim->par_size - 1.0,
                                       2.0*sim->par_size - 1.0)
                       );

  vtkm::cont::ArrayHandle<vtkm::Id> connectivity;
  vtkm::cont::ArrayCopy(vtkm::cont::make_ArrayHandleIndex(static_cast<vtkm::Id>(sim->n)), connectivity);
  
  vtkm::IdComponent numberOfPointsPerCell = 1;
  dataSet = dataSetBuilder.Create(positions2, // use template line 230
                                  vtkm::CellShapeTagVertex(),
                                  numberOfPointsPerCell,
                                  connectivity, "coords");

#ifdef STRIDED_SCALARS
  // NOTE: In this case, the num_vals, needs to be
  // the full extent of the strided area, thus sim->n*sim->NbofScalarfields
  std::cout << "creating fields with strided access\n";

  vtkm::cont::ArrayHandleStride<T> aos1 (AOS, sim->n, sim->NbofScalarfields, 7);
  dataSet.AddPointField("rho", aos1);
  
  vtkm::cont::ArrayHandleStride<T> aos2(AOS, sim->n, sim->NbofScalarfields, 8);
  dataSet.AddPointField("temp", aos2);
  
  vtkm::cont::ArrayHandleStride<T> aos3(AOS, sim->n, sim->NbofScalarfields, 0);
  dataSet.AddPointField("mass", aos3);
  
  dataSet.AddPointField("x", pos_x);
  dataSet.AddPointField("y", pos_y);
  dataSet.AddPointField("z", pos_z);

  vtkm::cont::ArrayHandleStride<T> vx(AOS, sim->n, sim->NbofScalarfields, 4);
  vtkm::cont::ArrayHandleStride<T> vy(AOS, sim->n, sim->NbofScalarfields, 5);
  vtkm::cont::ArrayHandleStride<T> vz(AOS, sim->n, sim->NbofScalarfields, 6);
  /* first method to view a vector field from its base components */
  auto velArray = vtkm::cont::make_ArrayHandleCompositeVector(vx, vy, vz);
  //vtkm::cont::printSummary_ArrayHandle(velArray, std::cout);
  dataSet.AddPointField("velocity", velArray);
#else
  std::cout << "creating fields with independent (stride=1) access\n";
//https://vtk-m.readthedocs.io/en/stable/basic-array-handles.html#ex-arrayhandlefromvector

  auto dataArray1 = vtkm::cont::make_ArrayHandle<T>(sim->rho, vtkm::CopyFlag::Off);
  dataSet.AddPointField("rho", dataArray1);

  auto dataArrayx = vtkm::cont::make_ArrayHandle<T>(sim->x, vtkm::CopyFlag::Off);
  dataSet.AddPointField("x", dataArrayx);
    auto dataArrayy = vtkm::cont::make_ArrayHandle<T>(sim->y, vtkm::CopyFlag::Off);
  dataSet.AddPointField("y", dataArrayy);
    auto dataArrayz = vtkm::cont::make_ArrayHandle<T>(sim->z, vtkm::CopyFlag::Off);
  dataSet.AddPointField("z", dataArrayz);
  
  auto dataArray2 = vtkm::cont::make_ArrayHandle(sim->temp, vtkm::CopyFlag::Off);
  dataSet.AddPointField("Temperature", dataArray2);
  
  auto dataArray3 = vtkm::cont::make_ArrayHandle(sim->mass, vtkm::CopyFlag::Off);
  dataSet.AddPointField("mass", dataArray3);

  auto vx = vtkm::cont::make_ArrayHandle(sim->vx, vtkm::CopyFlag::Off); 
  auto vy = vtkm::cont::make_ArrayHandle(sim->vy, vtkm::CopyFlag::Off); 
  auto vz = vtkm::cont::make_ArrayHandle(sim->vz, vtkm::CopyFlag::Off);
    /*
  // first method to view a vector field from its base components
  auto velArray = vtkm::cont::make_ArrayHandleCompositeVector(vx, vy, vz);
  vtkm::cont::printSummary_ArrayHandle(velArray, std::cout);
  dataSet.AddPointField("velocity", velArray);
  */
  // second method to view a vector field from its base components
  auto velArray2 = vtkm::cont::make_ArrayHandleSOA(vx, vy, vz);
  vtkm::cont::printSummary_ArrayHandle(velArray2, std::cout);
  dataSet.AddPointField("velocity2", velArray2);
   //
#endif

  //dataSet.PrintSummary(std::cout);
  
    //Creating Actor
  vtkm::cont::ColorTable colorTable("viridis");
  vtkm::rendering::Actor actor(dataSet.GetCellSet(),
                               dataSet.GetCoordinateSystem(),
                               dataSet.GetField("rho"),
                               colorTable);

  // Adding Actor to the scene
  scene.AddActor(actor);

  mapper0.SetUsePoints();
  mapper0.SetRadius(0.02f);
  mapper0.UseVariableRadius(false);
  mapper0.SetRadiusDelta(0.05f);
}

template<typename T>
void Execute([[maybe_unused]]int it, [[maybe_unused]]int frequency, sph::ParticlesData<T> *sim)
{
  std::ostringstream fname;
  if(it % frequency == 0)
    {
//#define RENDERING 1 // works only in non-strided mode
#ifdef  RENDERING
    fname << "/dev/shm/insitu." << std::setfill('0') << std::setw(4) << it << ".png";
    vtkm::rendering::View3D view(scene, mapper0, canvas);

    view.GetCamera().ResetToBounds(bounds);
    view.GetCamera().Azimuth(30.0);
    view.GetCamera().Elevation(30.0);
    view.SetBackgroundColor(vtkm::rendering::Color(1.0f, 1.0f, 1.0f));
    view.SetForegroundColor(vtkm::rendering::Color(0.0f, 0.0f, 0.0f));
    view.Paint();
    view.SaveAs(fname.str());
#endif

#define HISTSAMPLING 1
#ifdef  HISTSAMPLING
  /********** HistSampling *****************/
  using AssocType = vtkm::cont::Field::Association;
  vtkm::filter::resampling::HistSampling histsample;
  histsample.SetNumberOfBins(128);
  histsample.SetSampleFraction(0.05);
  histsample.SetActiveField("rho", AssocType::Points);
  auto histsampleDataSet = histsample.Execute(dataSet);
  if(1)
    {
    vtkm::filter::geometry_refinement::ConvertToPointCloud topc;
    topc.SetFieldsToPass("rho");
    auto topcDataSet = topc.Execute(histsampleDataSet);
  
    fname.str("");
    fname << "/dev/shm/histsample." << std::setfill('0') << std::setw(2) << sim->par_rank << "." << std::setfill('0') << std::setw(4) << it << ".vtk";
    std::cerr << "writing " << fname.str() << std::endl;
    vtkm::io::VTKDataSetWriter histsampleWriter(fname.str());
    histsampleWriter.SetFileTypeToBinary();
    histsampleWriter.WriteDataSet(topcDataSet);
    }
#endif

#define CROSS_SLICING 1
#ifdef  CROSS_SLICING
  vtkm::filter::entity_extraction::ThresholdPoints thresholdPoints;
  // 
  thresholdPoints.SetThresholdBetween(-0.01, 0.01);
  thresholdPoints.SetActiveField("z");
  thresholdPoints.SetFieldsToPass("rho");
  thresholdPoints.SetCompactPoints(true);
  auto output = thresholdPoints.Execute(dataSet);
  if(1)
    {
    fname.str("");
    fname << "/dev/shm/thresholdPoints." << std::setfill('0') << std::setw(2) << sim->par_rank << "." << std::setfill('0') << std::setw(4) << it << ".vtk";
    std::cerr << "writing " << fname.str() << std::endl;
    vtkm::io::VTKDataSetWriter writer(fname.str());
    writer.SetFileTypeToBinary();
    writer.WriteDataSet(output);
    }
#endif
    }
}

//#define DATADUMP 1
template<typename T>
void Finalize(const sph::ParticlesData<T> *sim)
{
#ifdef DATADUMP
  std::ostringstream fname;
  fname << "/dev/shm/finaldataset." << std::setfill('0') << std::setw(2) << sim->par_rank << ".vtk";

  vtkm::io::VTKDataSetWriter writer(fname.str());
  writer.SetFileTypeToBinary();
  writer.WriteDataSet(dataSet);
#endif
}
}
#endif


#pragma once

#include <iostream>
#include "solvers.h"

#ifdef USE_CATALYST
#include <catalyst.hpp>
#include <conduit_blueprint.h>
#include "CatalystAdaptor.h"
#endif

#ifdef USE_ASCENT
#include <ascent/ascent.hpp>
#include "AscentAdaptor.h"
#endif

#ifdef USE_VTKM
#include "VTKmAdaptor.h"
#endif

#include "timer.hpp"

namespace viz
{

template<typename T>
void init([[maybe_unused]] int argc,
               [[maybe_unused]] char** argv,
               [[maybe_unused]]  sph::ParticlesData<T> *sim)
{
#ifdef USE_CATALYST
  CatalystAdaptor::Initialize(argc, argv);
  std::cout << "CatalystInitialize\n";
#endif
#ifdef USE_ASCENT
  AscentAdaptor::Initialize(sim);
  std::cout << "AscentInitialize\n";
#endif
#ifdef USE_VTKM
  VTKmAdaptor::Initialize(argc, argv, sim);
  std::cout << "VTK-mInitialize\n";
#endif
}

template<typename T>
void execute([[maybe_unused]] sph::ParticlesData<T> *sim,
             [[maybe_unused]] int iteration,
             [[maybe_unused]] int frequency)
{
#ifdef USE_CATALYST
  CatalystAdaptor::Execute(sim);
#endif
#ifdef USE_ASCENT
  AscentAdaptor::Execute(iteration, frequency);
#endif
#ifdef USE_VTKM
  VTKmAdaptor::Execute(iteration, frequency);
#endif
}

template<typename T>
void finalize([[maybe_unused]] const sph::ParticlesData<T> *sim)
{
#ifdef USE_CATALYST
  CatalystAdaptor::Finalize();
#endif
#ifdef USE_ASCENT
  std::cout << "AscentFinalize\n";
  AscentAdaptor::Finalize();
#endif
#ifdef USE_VTKM
  std::cout << "VTK-mFinalize\n";
  VTKmAdaptor::Finalize(sim);
#endif
}

} // namespace viz

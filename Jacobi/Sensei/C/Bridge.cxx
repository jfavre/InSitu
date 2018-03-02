#include "Bridge.h"
#include "JacobiDataAdaptor.h"

#include <ConfigurableAnalysis.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkDataObject.h>
#include <vector>

#include <timer/Timer.h>

namespace BridgeInternals
{
  static vtkSmartPointer<pjacobi::JacobiDataAdaptor> GlobalDataAdaptor;
  static vtkSmartPointer<sensei::ConfigurableAnalysis> GlobalAnalysisAdaptor;
  static MPI_Comm comm;
}

//-----------------------------------------------------------------------------
void bridge_initialize(MPI_Comm comm,
  int g_x, int g_y, int g_z,
  int l_x, int l_y, int l_z,
  uint64_t start_extents_x, uint64_t start_extents_y, uint64_t start_extents_z,
  const char* config_file)
{
  BridgeInternals::comm = comm;
  if (!BridgeInternals::GlobalDataAdaptor)
    {
    BridgeInternals::GlobalDataAdaptor = vtkSmartPointer<pjacobi::JacobiDataAdaptor>::New();
    }
  BridgeInternals::GlobalDataAdaptor->Initialize(
    g_x, g_y, g_z,
    l_x, l_y, l_z,
    start_extents_x, start_extents_y, start_extents_z);

  BridgeInternals::GlobalAnalysisAdaptor = vtkSmartPointer<sensei::ConfigurableAnalysis>::New();
  BridgeInternals::GlobalAnalysisAdaptor->Initialize(comm, config_file);
}

//-----------------------------------------------------------------------------
void bridge_update(int tstep, double time, double* temperature)
{
  BridgeInternals::GlobalDataAdaptor->SetDataTime(time);
  BridgeInternals::GlobalDataAdaptor->SetDataTimeStep(tstep);
  BridgeInternals::GlobalDataAdaptor->AddArray("temperature", temperature);
  BridgeInternals::GlobalAnalysisAdaptor->Execute(BridgeInternals::GlobalDataAdaptor);
  BridgeInternals::GlobalDataAdaptor->ReleaseData();
}

//-----------------------------------------------------------------------------
void bridge_finalize()
{
  BridgeInternals::GlobalAnalysisAdaptor = NULL;
  BridgeInternals::GlobalDataAdaptor = NULL;
  
  timer::PrintLog(std::cout, BridgeInternals::comm);
}

#include "Bridge.h"
#include "JacobiDataAdaptor.h"

#include <ConfigurableAnalysis.h>
#include <Timer.h>

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkDataObject.h>
#include <vector>
#include <iostream>

using std::cerr;
using std::endl;

namespace BridgeInternals
{
  static vtkSmartPointer<pjacobi::JacobiDataAdaptor> GlobalDataAdaptor;
  static vtkSmartPointer<sensei::ConfigurableAnalysis> GlobalAnalysisAdaptor;
  static MPI_Comm comm;
}

//-----------------------------------------------------------------------------
void bridge_initialize(MPI_Comm comm, simulation_data *sim, const char *config_file)
{
  BridgeInternals::comm = comm;

  // initialize the data adaptor
  BridgeInternals::GlobalDataAdaptor =
    vtkSmartPointer<pjacobi::JacobiDataAdaptor>::New();

  BridgeInternals::GlobalDataAdaptor->Initialize(sim);

  // initialize the analysis adaptor
  BridgeInternals::GlobalAnalysisAdaptor = vtkSmartPointer<sensei::ConfigurableAnalysis>::New();
  BridgeInternals::GlobalAnalysisAdaptor->Initialize(comm, config_file);
}

//-----------------------------------------------------------------------------
void bridge_update(simulation_data *sim)
{
  BridgeInternals::GlobalDataAdaptor->Update(sim);

  if (!BridgeInternals::GlobalAnalysisAdaptor->Execute(BridgeInternals::GlobalDataAdaptor))
    {
    cerr << "ERROR: Failed to execute the analysis" << endl;
    abort();
    }
  BridgeInternals::GlobalDataAdaptor->ReleaseData();
}

//-----------------------------------------------------------------------------
void bridge_finalize()
{
#if defined(SENSEI_2)
  BridgeInternals::GlobalAnalysisAdaptor->Finalize();
#endif

  BridgeInternals::GlobalAnalysisAdaptor = NULL;
  BridgeInternals::GlobalDataAdaptor = NULL;

  //timer::PrintLog(std::cout, BridgeInternals::comm);
}

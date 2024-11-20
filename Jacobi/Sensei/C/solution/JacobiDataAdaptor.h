#ifndef PJACOBI_DATAADAPTOR_H
#define PJACOBI_DATAADAPTOR_H

#include <DataAdaptor.h>
#include <mpi.h>
#include "solvers.h"
#include "vtkSmartPointer.h"
#include <map>
#include <string>
#include <cstdint>

class vtkDoubleArray;
class vtkImageData;
class vtkMultiBlockDataSet;

namespace pjacobi
{
/// DataAdaptor is an adaptor for the pjacobi simulation (miniapp).
/// Its purpose is to map the simulation datastructures to VTK
/// data model.
class JacobiDataAdaptor : public sensei::DataAdaptor
{
public:
  static JacobiDataAdaptor* New();
  senseiTypeMacro(JacobiDataAdaptor, sensei::DataAdaptor);

  /// Initialize the data adaptor.
  void Initialize(simulation_data *sim);

  void Update(simulation_data *sim);

  /// Set the pointers to simulation memory.
  void AddArray(const std::string& name, double* data);

  /// Clear all arrays.
  void ClearArrays();

// make use of sense 2.0 API
// see branch request_specific_meshes
#if defined(SENSEI_2)
  int GetNumberOfMeshes(unsigned int &numMeshes) override;
  int GetMeshName(unsigned int id, std::string &meshName) override;
  int GetMesh(const std::string &meshName, bool structureOnly,
    vtkDataObject *&mesh) override;
  int AddArray(vtkDataObject* mesh, const std::string &meshName,
    int association, const std::string &arrayName) override;
  int GetNumberOfArrays(const std::string &meshName, int association,
    unsigned int &numberOfArrays) override;
  int GetArrayName(const std::string &meshName, int association,
    unsigned int index, std::string &arrayName) override;
  int ReleaseData() override;

  int GetMeshHasGhostNodes(const std::string &meshName, bool &hasGhostNodes, int &nLayers) override;
  int AddGhostNodesArray(vtkDataObject* mesh, const std::string &meshName) override;
#else
  vtkDataObject* GetMesh(bool structure_only=false) override;
  bool AddArray(vtkDataObject* mesh, int association, const std::string& arrayname) override;
  unsigned int GetNumberOfArrays(int association) override;
  std::string GetArrayName(int association, unsigned int index) override;
  void ReleaseData() override;
#endif

protected:
  JacobiDataAdaptor();
  ~JacobiDataAdaptor();

  typedef std::map<std::string, double*> VariablesType;
  VariablesType Variables;

  typedef std::map<std::string, vtkSmartPointer<vtkDoubleArray> > ArraysType;
  ArraysType Arrays;

  vtkSmartPointer<vtkMultiBlockDataSet> Mesh;

  simulation_data *sim;
private:
  JacobiDataAdaptor(const JacobiDataAdaptor&); // not implemented.
  void operator=(const JacobiDataAdaptor&); // not implemented.
};

}

#endif

#ifndef PJACOBI_DATAADAPTOR_H
#define PJACOBI_DATAADAPTOR_H

#include <DataAdaptor.h>
#include "vtkSmartPointer.h"
#include <map>
#include <string>
#include <cstdint>

class vtkDoubleArray;
class vtkImageData;

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
  void Initialize(
    int g_x, int g_y, int g_z,
    int l_x, int l_y, int l_z,
    uint64_t start_extents_x, uint64_t start_extents_y, uint64_t start_extents_z);

  /// Set the pointers to simulation memory.
  void AddArray(const std::string& name, double* data);

  /// Clear all arrays.
  void ClearArrays();

  vtkDataObject* GetMesh(bool structure_only=false) override;
  bool AddArray(vtkDataObject* mesh, int association, const std::string& arrayname) override;
  unsigned int GetNumberOfArrays(int association) override;
  std::string GetArrayName(int association, unsigned int index) override;
  void ReleaseData() override;

protected:
  JacobiDataAdaptor();
  ~JacobiDataAdaptor();

  typedef std::map<std::string, double*> VariablesType;
  VariablesType Variables;

  typedef std::map<std::string, vtkSmartPointer<vtkDoubleArray> > ArraysType;
  ArraysType Arrays;

  vtkSmartPointer<vtkImageData> Mesh;
  int PointExtent[6];
  int WholeExtent[6];
private:
  JacobiDataAdaptor(const JacobiDataAdaptor&); // not implemented.
  void operator=(const JacobiDataAdaptor&); // not implemented.
};

}

#endif

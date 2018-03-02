#include "JacobiDataAdaptor.h"

#include "vtkDataObject.h"
#include "vtkDoubleArray.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"

namespace pjacobi
{
//-----------------------------------------------------------------------------
senseiNewMacro(JacobiDataAdaptor);

//-----------------------------------------------------------------------------
JacobiDataAdaptor::JacobiDataAdaptor()
{
}

//-----------------------------------------------------------------------------
JacobiDataAdaptor::~JacobiDataAdaptor()
{
}

//-----------------------------------------------------------------------------
void JacobiDataAdaptor::Initialize(
  int g_x, int g_y, int g_z,
  int l_x, int l_y, int l_z,
  uint64_t start_extents_x, uint64_t start_extents_y, uint64_t start_extents_z)
{
  // we only really need to save the local extents for our current example. So
  // we'll just save that.
  this->PointExtent[0] = start_extents_x;
  this->PointExtent[1] = start_extents_x + l_x - 1;
  this->PointExtent[2] = start_extents_y;
  this->PointExtent[3] = start_extents_y + l_y - 1;
  this->PointExtent[4] = 0; // we are in 2-D
  this->PointExtent[5] = 0; // we are in 2-D

  // This is point-based.
  this->WholeExtent[0] = 0;
  this->WholeExtent[1] = g_x;
  this->WholeExtent[2] = 0;
  this->WholeExtent[3] = g_y;
  this->WholeExtent[4] = 0;
  this->WholeExtent[5] = g_z;
  this->GetInformation()->Set(vtkDataObject::DATA_EXTENT(),
    this->WholeExtent, 6);
}

//-----------------------------------------------------------------------------
void JacobiDataAdaptor::AddArray(const std::string& name, double* data)
{
  if (this->Variables[name] != data)
    {
    this->Variables[name] = data;
    this->Arrays.erase(name);
    }
}

//-----------------------------------------------------------------------------
void JacobiDataAdaptor::ClearArrays()
{
  this->Variables.clear();
  this->Arrays.clear();
}

//-----------------------------------------------------------------------------
vtkDataObject* JacobiDataAdaptor::GetMesh(bool vtkNotUsed(structure_only))
{
  if (!this->Mesh)
    {
    this->Mesh = vtkSmartPointer<vtkImageData>::New();
    this->Mesh->SetExtent(
      this->PointExtent[0], this->PointExtent[1],
      this->PointExtent[2], this->PointExtent[3],
      this->PointExtent[4], this->PointExtent[5]);
/*
std::cout << "SetExtent="<< this->PointExtent[0] << ", " << this->PointExtent[1] << ", " <<
                            this->PointExtent[2] << ", " << this->PointExtent[3] << ", " <<
                            this->PointExtent[4] << ", " << this->PointExtent[5] << ", " <<
std::endl;
*/
    }
  return this->Mesh;
}

//-----------------------------------------------------------------------------
bool JacobiDataAdaptor::AddArray(vtkDataObject* mesh, int association, const std::string& name)
{
  if (association != vtkDataObject::FIELD_ASSOCIATION_POINTS || name.empty())
    {
    return false;
    }

  VariablesType::iterator iterV = this->Variables.find(name);
  if (iterV == this->Variables.end())
    {
    return false;
    }
#ifndef NDEBUG
  vtkImageData* image = vtkImageData::SafeDownCast(mesh);
#endif
  assert(image != NULL);

  ArraysType::iterator iterA = this->Arrays.find(iterV->first);
  if (iterA == this->Arrays.end())
    {
    vtkSmartPointer<vtkDoubleArray>& vtkarray = this->Arrays[iterV->first];
    vtkarray = vtkSmartPointer<vtkDoubleArray>::New();
    vtkarray->SetName(name.c_str());
    const vtkIdType size = (this->PointExtent[1] - this->PointExtent[0] + 1) *
      (this->PointExtent[3] - this->PointExtent[2] + 1) *
      (this->PointExtent[5] - this->PointExtent[4] + 1);
    assert(size == image->GetNumberOfPoints());
    vtkarray->SetArray(iterV->second, size, 1);
    vtkImageData::SafeDownCast(mesh)->GetPointData()->AddArray(vtkarray);
    return true;
    }
  return true;
}

//-----------------------------------------------------------------------------
unsigned int JacobiDataAdaptor::GetNumberOfArrays(int association)
{
  return (association == vtkDataObject::FIELD_ASSOCIATION_POINTS)?
    static_cast<unsigned int>(this->Variables.size()): 0;
}

//-----------------------------------------------------------------------------
std::string JacobiDataAdaptor::GetArrayName(int association, unsigned int index)
{
  if (association != vtkDataObject::FIELD_ASSOCIATION_POINTS)
    {
    return std::string();
    }
  unsigned int count = 0;
  for (VariablesType::iterator iter=this->Variables.begin(), max=this->Variables.end();
    iter != max; ++iter, ++count)
    {
    if (count==index)
      {
      return iter->first;
      }
    }
  return std::string();
}

//-----------------------------------------------------------------------------
void JacobiDataAdaptor::ReleaseData()
{
  this->ClearArrays();
  this->Mesh = NULL;
}

}

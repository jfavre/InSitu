#include "JacobiDataAdaptor.h"

#include <vtkDataObject.h>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>

#include <Error.h>
#if defined(SENSEI_2)
#include <VTKUtils.h>
#endif

#include <mpi.h>

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
void JacobiDataAdaptor::Initialize(int m, int rankx, int ranky, int bx, int by, int ng)
{
  (void)m; // m could be used for whole extents [0 m+2 0 m+2 0 0]

  this->Origin[0] = -double(ng);
  this->Origin[1] = -double(ng);
  this->Origin[2] = 0.0;

  this->Spacing[0] = 1.0;
  this->Spacing[1] = 1.0;
  this->Spacing[2] = 1.0;

  this->Extent[0] = rankx*(bx - 1);
  this->Extent[1] = this->Extent[0] + bx + 2*ng - 1;
  this->Extent[2] = ranky*by;
  this->Extent[3] = this->Extent[2] + by + 2*ng - 1;
  this->Extent[4] = 0;
  this->Extent[5] = 0;
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

// make use of sense 2.0 API
// see branch request_specific_meshes
#if defined(SENSEI_2)
//-----------------------------------------------------------------------------
int JacobiDataAdaptor::GetNumberOfMeshes(unsigned int &numMeshes)
{
  // TODO -- retrun the number of meshes
}

//-----------------------------------------------------------------------------
int JacobiDataAdaptor::GetMeshName(unsigned int id, std::string &meshName)
{
  // TODO -- return the mesh name for the given mesh index
  // optionally make sure the index is valid
}

//-----------------------------------------------------------------------------
int JacobiDataAdaptor::GetNumberOfArrays(const std::string &meshName, int association,
  unsigned int &numberOfArrays)
{
  // TODO -- return the number of point or cell arrays for the
  // named mesh
}

//-----------------------------------------------------------------------------
int JacobiDataAdaptor::GetArrayName(const std::string &meshName, int association,
  unsigned int index, std::string &arrayName)
{
  // TODO -- return the point or cell array coresponding to index
  // on the named mesh
}

//-----------------------------------------------------------------------------
int JacobiDataAdaptor::GetMesh(const std::string &meshName, bool structureOnly,
  vtkDataObject *&mesh)
{
  // TODO -- construct and return a multiblock dataset
  // with MPI comm size number of blocks and this ranks
  // block set to a new vtk image data
}

//-----------------------------------------------------------------------------
int JacobiDataAdaptor::AddArray(vtkDataObject* mesh, const std::string &meshName,
  int association, const std::string &arrayName)
{
  // TODO -- add the named point or cell data array to the mesh
  // first cast the mesh into a multi block then get this ranks
  // block and cast it into an image. look up the pointer we
  // have cached and use VTK's zero copy mechanism to add it to
  // the image
}

//-----------------------------------------------------------------------------
int JacobiDataAdaptor::ReleaseData()
{
  // TODO -- clear chached data
}
#else

//-----------------------------------------------------------------------------
unsigned int JacobiDataAdaptor::GetNumberOfArrays(int association)
{
  // TODO -- return the number of point or cell arrays for the
  // named mesh
}

//-----------------------------------------------------------------------------
std::string JacobiDataAdaptor::GetArrayName(int association, unsigned int index)
{
  // TODO -- return the point or cell array coresponding to index
  // on the named mesh
}

//-----------------------------------------------------------------------------
vtkDataObject* JacobiDataAdaptor::GetMesh(bool vtkNotUsed(structure_only))
{
  // TODO -- construct and return a multiblock dataset
  // with MPI comm size number of blocks and this ranks
  // block set to a new vtk image data
}

//-----------------------------------------------------------------------------
bool JacobiDataAdaptor::AddArray(vtkDataObject* mesh, int association, const std::string& name)
{
  // TODO -- add the named point or cell data array to the mesh
  // first cast the mesh into a multi block then get this ranks
  // block and cast it into an image. look up the pointer we
  // have cached and use VTK's zero copy mechanism to add it to
  // the image
}

//-----------------------------------------------------------------------------
void JacobiDataAdaptor::ReleaseData()
{
  // TODO -- clear chached data
}
#endif

}

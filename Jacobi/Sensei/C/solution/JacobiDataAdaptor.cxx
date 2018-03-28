#include "JacobiDataAdaptor.h"

#include <vtkDataObject.h>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkRectilinearGrid.h>
#include <vtkUnsignedCharArray.h>

#include <Error.h>
#if defined(SENSEI_2)
#include <VTKUtils.h>
#endif

#include <mpi.h>

#define USE_IMAGE_DATA

namespace pjacobi
{
//-----------------------------------------------------------------------------
senseiNewMacro(JacobiDataAdaptor);

//-----------------------------------------------------------------------------
JacobiDataAdaptor::JacobiDataAdaptor()
{
    this->sim = NULL;
}

//-----------------------------------------------------------------------------
JacobiDataAdaptor::~JacobiDataAdaptor()
{
}

//-----------------------------------------------------------------------------
void JacobiDataAdaptor::Initialize(simulation_data *s)
{
    this->sim = s;
}

//-----------------------------------------------------------------------------
void JacobiDataAdaptor::Update(simulation_data *s)
{
    this->sim = s;

    this->SetDataTime(sim->iter*1.);
    this->SetDataTimeStep(sim->iter);
    this->AddArray("temperature", sim->Temp);
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
   numMeshes = 1;
   return 0;
}

//-----------------------------------------------------------------------------
int JacobiDataAdaptor::GetMeshName(unsigned int id, std::string &meshName)
{
  if (id > 0)
    {
    SENSEI_ERROR("index out of bounds")
    return -1;
    }

  meshName = "mesh";
  return 0;
}

//-----------------------------------------------------------------------------
int JacobiDataAdaptor::GetMesh(const std::string &meshName, bool structureOnly,
  vtkDataObject *&mesh)
{
  (void)structureOnly;

  if (meshName != "mesh")
    {
    SENSEI_ERROR("No mesh named " << meshName)
    return -1;
    }

  if (!this->Mesh)
    {
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n_ranks = 1;
    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

#ifdef USE_IMAGE_DATA
    int Extent[6];
    double Origin[3], Spacing[3];
    Extent[0] = this->sim->rankx*this->sim->bx;
    Extent[1] = Extent[0] + this->sim->bx + 2 - 1;
    Extent[2] = this->sim->ranky*this->sim->by;
    Extent[3] = Extent[2] + this->sim->by + 2 - 1;
    Extent[4] = 0;
    Extent[5] = 0;

    Spacing[0] = this->sim->cx[1] - this->sim->cx[0];
    Spacing[1] = this->sim->cy[1] - this->sim->cy[0];
    Spacing[2] = 0.;

    // Origin on rank 0 using cx[0], cy[0] is zero.
    Origin[0] = 0.;
    Origin[1] = 0.;
    Origin[2] = 0.;

    vtkImageData *block = vtkImageData::New();
    block->SetExtent(Extent);
    block->SetOrigin(Origin);
    block->SetSpacing(Spacing);
#else
    vtkRectilinearGrid *block = vtkRectilinearGrid::New();
    int dims[3];
    dims[0] = this->sim->bx+2;
    dims[1] = this->sim->by+2;
    dims[2] = 1;
    block->SetDimensions(dims);

    vtkFloatArray *x = vtkFloatArray::New();
    vtkFloatArray *y = vtkFloatArray::New();
    vtkFloatArray *z = vtkFloatArray::New();
    int dontDelete = 1;
    x->SetArray(this->sim->cx, this->sim->bx+2, dontDelete);
    y->SetArray(this->sim->cy, this->sim->by+2, dontDelete);
    z->SetNumberOfTuples(1);
    z->SetTuple1(0,0.);
    block->SetXCoordinates(x);
    block->SetYCoordinates(y);
    block->SetZCoordinates(z);
    x->Delete();
    y->Delete();
    z->Delete();
#endif
    this->Mesh = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    this->Mesh->SetNumberOfBlocks(n_ranks);
    this->Mesh->SetBlock(rank, block);

    block->Delete();
    }

  mesh = this->Mesh;

  return 0;
}

//-----------------------------------------------------------------------------
int JacobiDataAdaptor::AddArray(vtkDataObject* mesh, const std::string &meshName,
  int association, const std::string &arrayName)
{
  vtkMultiBlockDataSet *mb = dynamic_cast<vtkMultiBlockDataSet*>(mesh);
  if (!mb)
    {
    SENSEI_ERROR("Invalid mesh type " << mesh->GetClassName())
    return -1;
    }

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

#ifdef USE_IMAGE_DATA
  vtkImageData *block = dynamic_cast<vtkImageData*>(mb->GetBlock(rank));
#else
  vtkRectilinearGrid *block = dynamic_cast<vtkRectilinearGrid*>(mb->GetBlock(rank));
#endif
  if (!block)
    return 0;

  if (meshName != "mesh")
    {
    SENSEI_ERROR("No mesh named " << meshName)
    return -1;
    }

  if ((association != vtkDataObject::FIELD_ASSOCIATION_POINTS) ||
    (arrayName != "temperature"))
    {
    SENSEI_ERROR("No " << sensei::VTKUtils::GetAttributesName(association)
      << " data array named " << arrayName)
    return -1;
    }

  VariablesType::iterator iterV = this->Variables.find(arrayName);
  if (iterV == this->Variables.end())
    {
    return -1;
    }

  ArraysType::iterator iterA = this->Arrays.find(iterV->first);
  if (iterA == this->Arrays.end())
    {
    vtkSmartPointer<vtkDoubleArray>& vtkarray = this->Arrays[iterV->first];
    vtkarray = vtkSmartPointer<vtkDoubleArray>::New();
    vtkarray->SetName(arrayName.c_str());
    vtkIdType size = (this->sim->bx+2) * (this->sim->by+2);
    vtkarray->SetArray(iterV->second, size, 1);

    block->GetPointData()->SetScalars(vtkarray);
    }

  return 0;
}

//-----------------------------------------------------------------------------
int JacobiDataAdaptor::GetNumberOfArrays(const std::string &meshName, int association,
  unsigned int &numberOfArrays)
{
  if (meshName != "mesh")
    {
    SENSEI_ERROR("No mesh named " << meshName)
    return -1;
    }

  numberOfArrays = (association == vtkDataObject::FIELD_ASSOCIATION_POINTS) ?
    static_cast<unsigned int>(this->Variables.size()): 0;

  return 0;
}

//-----------------------------------------------------------------------------
int JacobiDataAdaptor::GetArrayName(const std::string &meshName, int association,
  unsigned int index, std::string &arrayName)
{
  if (meshName != "mesh")
    {
    SENSEI_ERROR("No mesh named " << meshName)
    return -1;
    }

  if (association != vtkDataObject::FIELD_ASSOCIATION_POINTS)
    {
    SENSEI_ERROR("No " << sensei::VTKUtils::GetAttributesName(association)
      << " data arrays")
    return -1;
    }

  if (index >= this->Variables.size())
    {
    SENSEI_ERROR("Index out of bounds")
    return -1;
    }

  VariablesType::iterator iter = this->Variables.begin();
  for (unsigned int i = 0; i < index; ++i)
    ++iter;

  arrayName = iter->first;

  return 0;
}

int JacobiDataAdaptor::GetMeshHasGhostNodes(const std::string &meshName, 
  bool &hasGhostNodes, int &nLayers)
{
  if (meshName != "mesh")
    {
    hasGhostNodes = false;
    nLayers = 0;
    SENSEI_ERROR("No mesh named " << meshName)
    return -1;
    }

  hasGhostNodes = true;
  nLayers = 1;
  return 0;
}

int JacobiDataAdaptor::AddGhostNodesArray(vtkDataObject* mesh, const std::string &meshName)
{
  if (meshName != "mesh")
    {
    SENSEI_ERROR("No mesh named " << meshName)
    return -1;
    }

  vtkMultiBlockDataSet *mb = dynamic_cast<vtkMultiBlockDataSet*>(mesh);
  if (!mb)
    {
    SENSEI_ERROR("Invalid mesh type " << mesh->GetClassName())
    return -1;
    }

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

#ifdef USE_IMAGE_DATA
  vtkImageData *block = dynamic_cast<vtkImageData*>(mb->GetBlock(rank));
#else
  vtkRectilinearGrid *block = dynamic_cast<vtkRectilinearGrid*>(mb->GetBlock(rank));
#endif
  if (!block)
    return -1;

  int nx = this->sim->bx+2;
  int ny = this->sim->by+2;
  vtkUnsignedCharArray *gn = vtkUnsignedCharArray::New();
  gn->SetNumberOfTuples(nx*ny);
  gn->SetName(GHOST_NODE_ARRAY_NAME().c_str());
  unsigned char *gptr = (unsigned char *)gn->GetVoidPointer(0);
  memset(gn->GetVoidPointer(0), 0, nx*ny*sizeof(unsigned char));
  unsigned char ghost = 1;
  for(int i = 0, j = 0 ; j < ny; ++j)
      gptr[j*nx+i] = ghost;
  for(int i = nx-1, j = 0 ; j < ny; ++j)
      gptr[j*nx+i] = ghost;
  for(int i = 0, j = 0 ; i < nx; ++i)
      gptr[j*nx+i] = ghost;
  for(int i = 0, j = ny-1 ; i < nx; ++i)
      gptr[j*nx+i] = ghost;

  block->GetPointData()->AddArray(gn);
  gn->Delete();

  return 0;
}

//-----------------------------------------------------------------------------
int JacobiDataAdaptor::ReleaseData()
{
  this->ClearArrays();
  this->Mesh = NULL;
  return 0;
}
#else

//-----------------------------------------------------------------------------
vtkDataObject* JacobiDataAdaptor::GetMesh(bool vtkNotUsed(structure_only))
{
  if (!this->Mesh)
    {
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n_ranks = 1;
    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

#ifdef USE_IMAGE_DATA
    int Extent[6];
    double Origin[3], Spacing[3];
    Extent[0] = this->sim->rankx*this->sim->bx;
    Extent[1] = Extent[0] + this->sim->bx + 2 - 1;
    Extent[2] = this->sim->ranky*this->sim->by;
    Extent[3] = Extent[2] + this->sim->by + 2 - 1;
    Extent[4] = 0;
    Extent[5] = 0;

    Spacing[0] = this->sim->cx[1] - this->sim->cx[0];
    Spacing[1] = this->sim->cy[1] - this->sim->cy[0];
    Spacing[2] = 0.;

    // Origin on rank 0 using cx[0], cy[0] is zero.
    Origin[0] = 0.;
    Origin[1] = 0.;
    Origin[2] = 0.;

    vtkImageData *block = vtkImageData::New();
    block->SetExtent(Extent);
    block->SetOrigin(Origin);
    block->SetSpacing(Spacing);
#else
    vtkRectilinearGrid *block = vtkRectilinearGrid::New();
    int dims[3];
    dims[0] = this->sim->bx+2;
    dims[1] = this->sim->by+2;
    dims[2] = 1;
    block->SetDimensions(dims);

    vtkFloatArray *x = vtkFloatArray::New();
    vtkFloatArray *y = vtkFloatArray::New();
    vtkFloatArray *z = vtkFloatArray::New();
    int dontDelete = 1;
    x->SetArray(this->sim->cx, this->sim->bx+2, dontDelete);
    y->SetArray(this->sim->cy, this->sim->by+2, dontDelete);
    z->SetNumberOfTuples(1);
    z->SetTuple1(0,0.);
    block->SetXCoordinates(x);
    block->SetYCoordinates(y);
    block->SetZCoordinates(z);
    x->Delete();
    y->Delete();
    z->Delete();
#endif

    this->Mesh = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    this->Mesh->SetNumberOfBlocks(n_ranks);
    this->Mesh->SetBlock(rank, block);

    block->Delete();
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

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  vtkMultiBlockDataSet *mb = dynamic_cast<vtkMultiBlockDataSet*>(mesh);
#ifdef USE_IMAGE_DATA
  vtkImageData *block = mb ? dynamic_cast<vtkImageData*>(mb->GetBlock(rank)) : nullptr;
#else
  vtkRectilinearGrid *block = mb ? dynamic_cast<vtkRectilinearGrid*>(mb->GetBlock(rank)) : nullptr;
#endif
  if (!block)
    return false;

  ArraysType::iterator iterA = this->Arrays.find(iterV->first);
  if (iterA == this->Arrays.end())
    {
    vtkSmartPointer<vtkDoubleArray>& vtkarray = this->Arrays[iterV->first];
    vtkarray = vtkSmartPointer<vtkDoubleArray>::New();
    vtkarray->SetName(name.c_str());
    vtkIdType size = (this->sim->bx+2) * (this->sim->by+2);
    vtkarray->SetArray(iterV->second, size, 1);
    block->GetPointData()->SetScalars(vtkarray);
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
#endif

}

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
    this->sim = NULL;
}

//-----------------------------------------------------------------------------
JacobiDataAdaptor::~JacobiDataAdaptor()
{
}

//-----------------------------------------------------------------------------
void JacobiDataAdaptor::Initialize(simulation_data *s) //int m, int rankx, int ranky, int bx_, int by_, int ng)
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
    this->Mesh = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    this->Mesh->SetNumberOfBlocks(1);
    this->Mesh->SetBlock(0, block);

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

  vtkRectilinearGrid *block = dynamic_cast<vtkRectilinearGrid*>(mb->GetBlock(0));
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
    this->Mesh = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    this->Mesh->SetNumberOfBlocks(1);
    this->Mesh->SetBlock(0, block);

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
  vtkRectilinearGrid *block = mb ? dynamic_cast<vtkRectilinearGrid*>(mb->GetBlock(0)) : nullptr;
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

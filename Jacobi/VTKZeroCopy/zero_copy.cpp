
#include <vtkStreamTracer.h>

#include <vtkNamedColors.h>
#include <vtkActor.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkProperty.h>

#include <vtkImageData.h>
#include <vtkDoubleArray.h>
#include <vtkSOADataArrayTemplate.h>
#include <vtkAOSDataArrayTemplate.h>
#include <vtkPointData.h>
#include <vtkGeometryFilter.h>
#include <vtkNew.h>

template<typename n_t>
void generate_data_soa(int nx, int ny, n_t *x0, n_t *dx, n_t *&vx, n_t *&vy, n_t *&vz)
{
    int nxy = nx*ny;
    vx = (n_t*)malloc(nxy*sizeof(n_t));
    vy = (n_t*)malloc(nxy*sizeof(n_t));
    vz = (n_t*)malloc(nxy*sizeof(n_t));
    for (int j = 0; j < ny; ++j)
    {
        for (int i = 0; i < nx; ++i)
        {
            int q = nx*j + i;

            n_t x = x0[0] + i*dx[0];
            n_t y = x0[1] + j*dx[1];
            n_t m = std::sqrt(x*x + y*y);

            vx[q] = -y/m;
            vy[q] =  x/m;
            vz[q] = n_t();
        }
    }
}

template<typename n_t>
void generate_data_aos(int nx, int ny, n_t *x0, n_t *dx, n_t *&v)
{
    int nxy = nx*ny;
    v = (n_t*)malloc(3*nxy*sizeof(n_t));
    for (int j = 0; j < ny; ++j)
    {
        for (int i = 0; i < nx; ++i)
        {
            int q = 3*(nx*j + i);

            n_t x = x0[0] + i*dx[0];
            n_t y = x0[1] + j*dx[1];
            n_t m = std::sqrt(x*x + y*y);

            v[q  ] = -y/m;
            v[q+1] =  x/m;
            v[q+2] =  n_t();
        }
    }
}

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    cerr << "error usage" << endl
      << "zero_copy [aos|soa] [nx] [ny]" << endl
      << endl;
    return -1;
  }

  std::string type = argv[1];
  int nx = atoi(argv[2]);
  int ny = atoi(argv[3]);
  int nxy = nx*ny;

  cerr << "running " << type << " " << nx << " x " << ny << " case" << endl;

  // generate a uniform Cartesian mesh from -1 to 1 in x and y
  double dx[3] = {2.0/double(nx), 2.0/double(ny), 0.0};
  double x0[3] = {-1.0, -1.0, 0.0};
  int ext[6] = {0, nx-1, 0, ny-1, 0, 0};

  vtkNew<vtkImageData> im;
  im->SetOrigin(x0);
  im->SetSpacing(dx);
  im->SetExtent(ext);

  if (type == "soa")
  {
      // structure of arrays case
      // a vector field in 3 arrays
      double *vx = nullptr;
      double *vy = nullptr;
      double *vz = nullptr;
      generate_data_soa(nx, ny, x0, dx, vx, vy, vz);

      // use the SOA class
      vtkSOADataArrayTemplate<double> *soa = vtkSOADataArrayTemplate<double>::New();
      soa->SetNumberOfComponents(3);
      soa->SetArray(0, vx, nxy, true);
      soa->SetArray(1, vy, nxy);
      soa->SetArray(2, vz, nxy);
      soa->SetName("soa");

      // add to the image as usual
      im->GetPointData()->AddArray(soa);
      im->GetPointData()->SetActiveVectors("soa");

      // git rid of our reference
      soa->Delete();

      // print the array
      soa->Print(cerr);
  }
  else if (type == "aos")
  {
      // array of structures case
      // aa vector field in a single array
      double *v = nullptr;
      generate_data_aos(nx, ny, x0, dx, v);

      // VTK's default is AOS, no need to use vtkAOSDataArrayTemplate
      vtkDoubleArray *aos = vtkDoubleArray::New();
      aos->SetNumberOfComponents(3);
      aos->SetArray(v, 3*nxy, 0);
      aos->SetName("aos");

      // add the array as usual
      im->GetPointData()->AddArray(aos);
      im->GetPointData()->SetActiveVectors("aos");

      // give up our reference
      aos->Delete();

      // print the array
      aos->Print(cerr);
  }
  else
  {
      cerr << "error: type must be \"soa\" or \"aos\"" << endl;
      return -1;
  }

  // compute a stream line
  // the domain is -1 to 1 in x and y and vector field is
  // tangent to concentric circles, so seed from 0.5,0.5
  vtkNew<vtkStreamTracer> streamLine;
  streamLine->SetInputData(im.GetPointer());
  streamLine->SetMaximumPropagation(100);
  streamLine->SetInitialIntegrationStep(0.1);
  streamLine->SetIntegrationDirectionToBoth();
  streamLine->SetStartPosition(0.5, 0.5, 0.0);

  // rendering code
  vtkNew<vtkGeometryFilter> geom;
  geom->SetInputData(im.GetPointer());

  vtkNew<vtkPolyDataMapper> geomMap;
  geomMap->SetInputConnection(geom->GetOutputPort());

  vtkNew<vtkActor> geomAct;
  geomAct->SetMapper(geomMap.GetPointer());
  geomAct->GetProperty()->EdgeVisibilityOn();

  vtkNew<vtkPolyDataMapper> streamLineMapper;
  streamLineMapper->SetInputConnection(streamLine->GetOutputPort());

  vtkNew<vtkNamedColors> namedColors;

  vtkNew<vtkActor> streamLineActor;
  streamLineActor->SetMapper(streamLineMapper.GetPointer());
  streamLineActor->VisibilityOn();
  streamLineActor->GetProperty()->SetColor(namedColors->GetColor3d("Red").GetData());


  // Create the RenderWindow, Renderer and Actors
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer.GetPointer());

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow.GetPointer());

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  interactor->SetInteractorStyle(style.GetPointer());

  renderer->AddActor(geomAct.GetPointer());
  renderer->AddActor(streamLineActor.GetPointer());

  // Add the actors to the renderer, set the background and size
  renderer->SetBackground(namedColors->GetColor3d("Cadet").GetData());
  renderWindow->SetSize(300, 300);
  interactor->Initialize();
  renderWindow->Render();

  interactor->Start();

  return 0;
}

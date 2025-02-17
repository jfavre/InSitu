#ifndef AscentAdaptor_h
#define AscentAdaptor_h

/*
publish() leads to following call path

VTKHDataAdapter::BlueprintToVTKHCollection()
VTKHDataAdapter::PointsImplicitBlueprintToVTKmDataSet()
        coords = detail::GetExplicitCoordinateSystem<float64>() which
        allocates connectivity.Allocate(nverts);
*/

#include "conduit_blueprint.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>

namespace AscentAdaptor
{
  ascent::Ascent ascent;
  conduit::Node  mesh;
  conduit::Node  actions;

template<typename T>
void addField(conduit::Node& mesh, const std::string& name, T* field, const size_t N)
{
    mesh["fields/" + name + "/association"] = "vertex";
    mesh["fields/" + name + "/topology"]    = "mesh";
    mesh["fields/" + name + "/values"].set_external(field, N);
    mesh["fields/" + name + "/volume_dependent"].set("false");
}

template<typename T>
void addStridedField(conduit::Node& mesh,
                     const std::string& name,
                     T* field,
                     const size_t N,  /* num_elements */
                     const int offset,
                     const int stride)
{
    mesh["fields/" + name + "/association"] = "vertex";
    mesh["fields/" + name + "/topology"]    = "mesh";
    mesh["fields/" + name + "/values"].set_external_float32_ptr(field, N,
                                                                offset * sizeof(T),
                                                                stride * sizeof(T));
    mesh["fields/" + name + "/volume_dependent"].set("false");
}



template<typename T>
void Initialize(sph::ParticlesData<T> *sim)
{
  conduit::Node n;
  ascent::about(n);
  // only run this test if ascent was built with vtkm support
  if(n["runtimes/ascent/vtkm/status"].as_string() == "disabled")
  {
    ASCENT_INFO("Ascent vtkm support disabled, skipping test");
    return;
  }
  
  std::string output_path = "datasets";
  ASCENT_INFO("Creating output folder: " + output_path);
  if(!conduit::utils::is_directory(output_path))
  {
    conduit::utils::create_directory(output_path);
  }
  // remove old images before rendering


  conduit::Node ascent_options;
  ascent_options["default_dir"] = "./datasets";
  ascent_options["mpi_comm"] = MPI_Comm_c2f(MPI_COMM_WORLD);
  ascent_options["ascent_info"] = "verbose";
  ascent_options["exceptions"] = "forward";

  ascent.open(ascent_options);

  mesh["state/cycle"].set_external(&sim->iteration);
  mesh["state/time"].set_external(&sim->time);
  mesh["state/domain_id"].set_external(&sim->par_rank);
  conduit::Node verify_info;

  std::cout << "time: " << sim->iteration*0.1 << " cycle: " << sim->iteration << std::endl;

  mesh["coordsets/coords/type"] = "explicit";
  mesh["topologies/mesh/coordset"] = "coords";
#ifdef STRIDED_SCALARS
  mesh["coordsets/coords/values/x"].set_external_float32_ptr(&sim->scalarsAOS[0].pos[0], sim->n,
                                                                0 * sizeof(T),
                                                                sim->NbofScalarfields * sizeof(T));
  mesh["coordsets/coords/values/y"].set_external_float32_ptr(&sim->scalarsAOS[0].pos[0], sim->n,
                                                                1 * sizeof(T),
                                                                sim->NbofScalarfields * sizeof(T));
  mesh["coordsets/coords/values/z"].set_external_float32_ptr(&sim->scalarsAOS[0].pos[0], sim->n,
                                                                2 * sizeof(T),
                                                                sim->NbofScalarfields * sizeof(T));
#else
  mesh["coordsets/coords/values/x"].set_external(sim->x);
  mesh["coordsets/coords/values/y"].set_external(sim->y);
  mesh["coordsets/coords/values/z"].set_external(sim->z);
#endif

#define IMPLICIT_CONNECTIVITY_LIST 1 // the connectivity list is not given, but created by vtkm
#ifdef  IMPLICIT_CONNECTIVITY_LIST
  mesh["topologies/mesh/type"] = "points";
#else
  mesh["topologies/mesh/type"] = "unstructured";
  std::vector<conduit_int32> conn(sim->n);
  std::iota(conn.begin(), conn.end(), 0);
  mesh["topologies/mesh/elements/connectivity"].set(conn);
  mesh["topologies/mesh/elements/shape"] = "point";
#endif
  
#ifdef STRIDED_SCALARS
  addStridedField(mesh, "rho",  &sim->scalarsAOS[0].rho, sim->n, 0, sim->NbofScalarfields);
  addStridedField(mesh, "temp", &sim->scalarsAOS[0].temp, sim->n, 0, sim->NbofScalarfields);
  addStridedField(mesh, "mass", &sim->scalarsAOS[0].mass, sim->n, 0, sim->NbofScalarfields);

  addStridedField(mesh, "x", &(sim->scalarsAOS[0].pos[0]), sim->n, 0, sim->NbofScalarfields);
  addStridedField(mesh, "y", &(sim->scalarsAOS[0].pos[0]), sim->n, 1, sim->NbofScalarfields);
  addStridedField(mesh, "z", &(sim->scalarsAOS[0].pos[0]), sim->n, 2, sim->NbofScalarfields);
  addStridedField(mesh, "vx", &(sim->scalarsAOS[0].vel[0]), sim->n, 0, sim->NbofScalarfields);
  addStridedField(mesh, "vy", &(sim->scalarsAOS[0].vel[0]), sim->n, 1, sim->NbofScalarfields);
  addStridedField(mesh, "vz", &(sim->scalarsAOS[0].vel[0]), sim->n, 2, sim->NbofScalarfields);

  /* there seems to be an offset I don't understand
  mesh["fields/velocity/association"] = "vertex";
  mesh["fields/velocity/topology"]    = "mesh";
  mesh["fields/velocity/values/u"].set_external_float32_ptr(&(sim->scalarsAOS[0].vel[0]), sim->n,
                                                                0,
                                                                sim->NbofScalarfields * sizeof(T));
  mesh["fields/velocity/values/v"].set_external_float32_ptr(&(sim->scalarsAOS[0].vel[0]), sim->n,
                                                                1 * sizeof(T),
                                                                sim->NbofScalarfields * sizeof(T));
  mesh["fields/velocity/values/w"].set_external_float32_ptr(&(sim->scalarsAOS[0].vel[0]), sim->n,
                                                                2 * sizeof(T),
                                                                sim->NbofScalarfields * sizeof(T));
  mesh["fields/velocity/volume_dependent"].set("false");
  */

#else
  addField(mesh, "rho" , sim->rho.data(), sim->n);
  addField(mesh, "temp", sim->temp.data(), sim->n);
  addField(mesh, "mass", sim->mass.data(), sim->n);

  addField(mesh, "vx", sim->vx.data(), sim->n);
  addField(mesh, "vy", sim->vy.data(), sim->n);
  addField(mesh, "vz", sim->vz.data(), sim->n);
  
  addField(mesh, "x", sim->x.data(), sim->n);
  addField(mesh, "y", sim->y.data(), sim->n);
  addField(mesh, "z", sim->z.data(), sim->n);

  /*
  mesh["fields/velocity/association"] = "vertex";
  mesh["fields/velocity/topology"]    = "mesh";
  mesh["fields/velocity/values/u"].set_external(sim->vx.data(), sim->n);
  mesh["fields/velocity/values/v"].set_external(sim->vy.data(), sim->n);
  mesh["fields/velocity/values/w"].set_external(sim->vz.data(), sim->n);
  mesh["fields/velocity/volume_dependent"].set("false");
  */
#endif

  if(!conduit::blueprint::mesh::verify(mesh,verify_info))
  {
    CONDUIT_INFO("blueprint verify failed!" + verify_info.to_json());
  }
  if(conduit::blueprint::mcarray::is_interleaved(mesh["coordsets/coords/values"]))
    std::cout << "Conduit Blueprint check found interleaved coordinates" << std::endl;
  else
    std::cout << "Conduit Blueprint check found contiguous coordinates" << std::endl;
  mesh.print();
// Create an action that tells Ascent to:
//  add a scene (s1) with one plot (p1)
//  that will render a pseudocolor of 
//  the mesh field `rho`

  conduit::Node &add_act = actions.append(); 
  add_act["action"] = "add_scenes";

// declare a scene (s1) and pseudocolor plot (p1)
  conduit::Node &scenes = add_act["scenes"];
  scenes["s1/plots/p1/type"] = "pseudocolor";
//#define RANKS
#ifdef RANKS
  scenes["s1/plots/p1/field"] = "ranks";
  scenes["s1/plots/p1/pipeline"] = "pl1";
  scenes["s1/plots/p1/color_table/discrete"] = "true";
#else
  scenes["s1/plots/p1/field"] = "rho";
#endif
  //scenes["s1/plots/p1/points/radius"] = .005;
  //scenes["s1/plots/p1/points/radius_delta"] = .01;
  scenes["s1/renders/r1/image_prefix"] = "image.%05d";
  scenes["s1/renders/r1/annotations"] = "true";
  double vec3[3];
  vec3[0] = (sim->par_size-1.0)/2; vec3[1] = (sim->par_size-1.0)/2; vec3[2] = 0.0;
  scenes["s1/renders/r1/camera/look_at"].set_float64_ptr(vec3,3);
  vec3[0] = (sim->par_size-1.0)/2; vec3[1] = (sim->par_size-1.0)/2; vec3[2] = 10;
  scenes["s1/renders/r1/camera/position"].set_float64_ptr(vec3,3);
  vec3[0] = 0.0; vec3[1] = 1; vec3[2] = 0.0;
  scenes["s1/renders/r1/camera/up"].set_float64_ptr(vec3,3);
  scenes["s1/renders/r1/camera/zoom"] = 5.0/sim->par_size;
  scenes["s1/renders/r1/image_width"] = 1024;
  scenes["s1/renders/r1/image_height"] = 1024;
  double dset_bounds[6] = {-1.0, 1.0 * sim->par_size, -1.0, 1.0 * sim->par_size, 0.0, 1.};
  scenes["s1/renders/r1/dataset_bounds"].set_float64_ptr(dset_bounds, 6);
/*
  conduit::Node pipelines;
  pipelines["pl1/f1/type"] = "add_mpi_ranks";
  conduit::Node &params = pipelines["pl1/f1/params"];
  params["topology"] = "mesh";
  params["output"] = "ranks";

  conduit::Node &add_pipelines = actions.append();
  add_pipelines["action"] = "add_pipelines";
  add_pipelines["pipelines"] = pipelines;
  */
}

void Execute([[maybe_unused]]int it, [[maybe_unused]]int frequency)
{
  if(it % frequency == 0)
    {
    ascent.publish(mesh);
    ascent.execute(actions);
    }
}

//#define DATADUMP 1
void Finalize()
{
#ifdef DATADUMP
  conduit::Node save_data_actions;
  conduit::Node &add_act = save_data_actions.append(); 
  add_act["action"] = "add_extracts";
  conduit::Node &extracts = add_act["extracts"];
  extracts["e1/type"] = "relay";
  extracts["e1/params/path"] = "mesh";
  extracts["e1/params/protocol"] = "hdf5";

  ascent.publish(mesh);
  ascent.execute(save_data_actions);
  ascent.close();
#endif
}

}
#endif

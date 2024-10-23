#ifndef CatalystAdaptor_h
#define CatalystAdaptor_h

#include "solvers.h"
#include <catalyst.hpp>
#include <conduit_blueprint.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>

namespace CatalystAdaptor
{
/**
 * In this example, we show how we can use Catalysts's C++
 * wrapper around conduit's C API to create Conduit nodes.
 * This is not required. A C++ adaptor can just as
 * conveniently use the Conduit C API to setup the
 * `conduit_node`. However, this example shows that one can
 * indeed use Catalyst's C++ API, if the developer so chooses.
 */
void Initialize(int argc, char* argv[])
{
  conduit_cpp::Node node;
  for (int cc = 1; cc < argc; ++cc)
  {
    if(strcmp(argv[cc], "--help") == 0 || strcmp(argv[cc], "-h") == 0)
    {
      std::cout << "Runtime Options: " << std::endl;
      std::cout << "               : --help or -h (help)" << std::endl;
      std::cout << "               : --catalyst script.py" << std::endl;
      std::cout << "               : --pv (choose the ParaView Catalyst)" << std::endl;
      exit(1);
    }
    else if (strcmp(argv[cc], "--output") == 0 && (cc + 1) < argc)
    {
      node["catalyst/pipelines/0/type"].set("io");
      node["catalyst/pipelines/0/filename"].set(argv[cc + 1]);
      node["catalyst/pipelines/0/channel"].set("grid");
      ++cc;
    }
    else if (strcmp(argv[cc], "--pv") == 0)
    {
      node["catalyst_load/implementation"].set_string("paraview");
      // node["catalyst_load/search_paths/paraview"] = PARAVIEW_IMPL_DIR; //legacy before 5.11
      std::cout << "Loading ParaView implementation\n";
    }
    else if (strcmp(argv[cc], "--catalyst") == 0 && (cc + 1) < argc)
    {
      const auto fname = std::string(argv[cc+1]);
      // note: one can simply add the script file as follows:
      // node["catalyst/scripts/script" + std::to_string(cc - 1)].set_string(path);

      // alternatively, use this form to pass optional parameters to the script.
      const auto path = "catalyst/scripts/script" + std::to_string(cc - 1);
      node[path + "/filename"].set_string(fname);
    }
  }

  catalyst_status err = catalyst_initialize(conduit_cpp::c_node(&node));
  if (err != catalyst_status_ok)
    std::cerr << "ERROR: Failed to initialize Catalyst: " << err << std::endl;
}

template<typename T>
void addField(conduit_cpp::Node& mesh, const std::string& name, std::vector<T> &field)
{
    mesh["fields/" + name + "/association"] = "vertex";
    mesh["fields/" + name + "/topology"]    = "mesh";
    mesh["fields/" + name + "/values"].set_external(field);
    mesh["fields/" + name + "/volume_dependent"].set("false");
}

void Execute(sph::ParticlesData& sim)
{
  conduit_cpp::Node exec_params;

  // add time/cycle information
  auto state = exec_params["catalyst/state"];
  state["timestep"].set(sim.iteration);
  state["time"].set(sim.iteration*0.1);

  // Add channels.
  // We only have 1 channel here. Let's name it 'grid'.
  auto channel = exec_params["catalyst/channels/grid"];

  // Since this example is using Conduit Mesh Blueprint to define the mesh,
  // we set the channel's type to "mesh".
  channel["type"].set("mesh");

  // now create the mesh.
  auto mesh = channel["data"];

  // start with coordsets
  mesh["coordsets/coords/type"].set("explicit");
  mesh["coordsets/coords/values/x"].set_external(sim.x);
  mesh["coordsets/coords/values/y"].set_external(sim.y);
  mesh["coordsets/coords/values/z"].set_external(sim.z);

#define IMPLICIT_POINTS_SUPPORTED 1
#ifdef IMPLICIT_POINTS_SUPPORTED
  mesh["topologies/mesh/type"] = "points";
#else
  mesh["topologies/mesh/type"] = "unstructured";
  std::vector<int> conn(sim.n);
  std::iota(conn.begin(), conn.end(), 0);
  // use a deep-copy here otherwise the HDF5 output is corrupted. ??
  mesh["topologies/mesh/elements/connectivity"].set(conn);
  mesh["topologies/mesh/elements/shape"] = "point";
#endif

  mesh["topologies/mesh/coordset"].set("coords");
 
  // Finally, add fields.
  auto fields = mesh["fields"];
  
#ifdef STRIDED_SCALARS
  fields["Densities/association"].set("vertex");
  fields["Densities/topology"].set("mesh");
  fields["Densities/volume_dependent"].set("false");
  
  fields["Densities/values/x"].set_external(&sim.scalars[0],
                                                     sim.n /* num_elements */,
                                                     0 * sizeof(double), sim.NbofScalarfields * sizeof(double));
  fields["Densities/values/y"].set_external(&sim.scalars[0],
                                                     sim.n /* num_elements */,
                                                     1 * sizeof(double), sim.NbofScalarfields * sizeof(double));
  fields["Densities/values/z"].set_external(&sim.scalars[0],
                                                     sim.n /* num_elements */,
                                                     2 * sizeof(double), sim.NbofScalarfields * sizeof(double));
#else 
  addField(mesh, "Density1", sim.scalar1);
  addField(mesh, "Density2", sim.scalar2);
  addField(mesh, "Density3", sim.scalar3);
#endif
  addField(mesh, "x", sim.x);
  addField(mesh, "y", sim.y);
  addField(mesh, "z", sim.z);

  conduit_cpp::Node verify_info;
  if (!conduit_blueprint_verify("mesh", conduit_cpp::c_node(&mesh), conduit_cpp::c_node(&verify_info)))
    std::cerr << "ERROR: blueprint verify failed!" + verify_info.to_json() << std::endl;
  //else std::cerr << "PASS: blueprint verify passed!"<< std::endl;

  catalyst_status err = catalyst_execute(conduit_cpp::c_node(&exec_params));
  if (err != catalyst_status_ok)
  {
    std::cerr << "ERROR: Failed to execute Catalyst: " << err << std::endl;
  }
  mesh.print();
}

void Finalize()
{
  conduit_cpp::Node node;
  catalyst_status err = catalyst_finalize(conduit_cpp::c_node(&node));
  if (err != catalyst_status_ok)
    std::cerr << "ERROR: Failed to finalize Catalyst: " << err << std::endl;
}
}

#endif

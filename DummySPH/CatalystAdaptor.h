#ifndef CatalystAdaptor_h
#define CatalystAdaptor_h

#include "solvers.h"
#include <catalyst.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>

namespace CatalystAdaptor
{
static std::vector<std::string> filesToValidate;

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
  {
    std::cerr << "ERROR: Failed to initialize Catalyst: " << err << std::endl;
  }
}

void Execute(sph::ParticlesData& sim)
{
  conduit_cpp::Node exec_params;

  // add time/cycle information
  auto state = exec_params["catalyst/state"];
  state["timestep"].set(sim.iteration);
  state["time"].set(sim.iteration*0.1);

  // add optional execution parameters
  state["parameters"].append().set_string("parameter0");
  state["parameters"].append().set_string("parameter1=42");
  state["parameters"].append().set_string("parameter2=doThing");
  state["parameters"].append().set_string("timeParam=" + std::to_string(sim.iteration*0.1));

  // Add channels.
  // We only have 1 channel here. Let's name it 'grid'.
  auto channel = exec_params["catalyst/channels/grid"];

  // Since this example is using Conduit Mesh Blueprint to define the mesh,
  // we set the channel's type to "mesh".
  channel["type"].set("mesh");   //used to indicate that this channel is specified in accordance to the Conduit Mesh protocol.

  // now create the mesh.
  auto mesh = channel["data"];

  // start with coordsets
  mesh["coordsets/coords/type"].set("explicit");
  mesh["coordsets/coords/values/x"].set_external(sim.x);
  mesh["coordsets/coords/values/y"].set_external(sim.y);
  mesh["coordsets/coords/values/z"].set_external(sim.z);

  // Next, add topology
  mesh["topologies/mesh/type"].set("unstructured");
  mesh["topologies/mesh/elements/shape"].set("point");
  mesh["topologies/mesh/coordset"].set("coords");
  std::vector<conduit_int64> conn(sim.n);
  std::iota(conn.begin(), conn.end(), 0);
  mesh["topologies/mesh/elements/connectivity"].set(conn);
 
  // Finally, add fields.
  auto fields = mesh["fields"];
  // temperature is vertex-data.
  fields["rho/association"].set("vertex");
  fields["rho/topology"].set("mesh");
  fields["rho/volume_dependent"].set("false");
  // Conduit supports zero copy, allowing a Conduit Node to describe and
  // point to externally allocated data
  fields["rho/values"].set_external(sim.ro);
  
  catalyst_status err = catalyst_execute(conduit_cpp::c_node(&exec_params));
  if (err != catalyst_status_ok)
  {
    std::cerr << "ERROR: Failed to execute Catalyst: " << err << std::endl;
  }
  //exec_params.print();
}

void Finalize()
{
  conduit_cpp::Node node;
  catalyst_status err = catalyst_finalize(conduit_cpp::c_node(&node));
  if (err != catalyst_status_ok)
  {
    std::cerr << "ERROR: Failed to finalize Catalyst: " << err << std::endl;
  }

  for (const auto& fname : filesToValidate)
  {
    std::ifstream istrm(fname.c_str(), std::ios::binary);
    if (!istrm.is_open())
    {
      std::cerr << "ERROR: Failed to open file '" << fname.c_str() << "'." << std::endl;
    }
  }
}
}

#endif

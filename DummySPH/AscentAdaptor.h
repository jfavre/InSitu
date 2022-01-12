#ifndef AscentAdaptor_h
#define AscentAdaptor_h

#include "solvers.h"
#include <ascent/ascent.hpp>
#include "conduit_blueprint.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>

namespace AscentAdaptor
{
  ascent::Ascent a;
  conduit::Node mesh;
  conduit::Node actions;
        
void Initialize(int argc, char* argv[], sph::ParticlesData& sim)
{
  conduit::Node ascent_options;
  ascent_options["default_dir"] = "/users/jfavre/Projects/InSitu/DummySPH/buildAscent/datasets";
#if USE_MPI
  ascent_opts["mpi_comm"] = MPI_Comm_c2f(MPI_COMM_WORLD);
#endif
  a.open(ascent_options);

  mesh["state/cycle"].set_external(&sim.iteration);
  mesh["state/time"].set_external(&sim.time);
  std::cout << "time: " << sim.iteration*0.1 << " cycle: " << sim.iteration << std::endl;

  mesh["coordsets/coords/type"] = "explicit";
  mesh["coordsets/coords/values/x"].set_external(sim.x);
  mesh["coordsets/coords/values/y"].set_external(sim.y);
  mesh["coordsets/coords/values/z"].set_external(sim.z);

  mesh["topologies/mesh/type"] = "unstructured";
  mesh["topologies/mesh/coordset"] = "coords";

  mesh["fields/rho/association"] = "vertex";
  mesh["fields/rho/topology"] = "mesh";
  mesh["fields/rho/values"].set_external(sim.ro);
  mesh["fields/rho/volume_dependent"].set("false");
  
  std::vector<conduit_int64> conn(sim.n);
  std::iota(conn.begin(), conn.end(), 0);
  mesh["topologies/mesh/elements/connectivity"].set_external(conn);
  mesh["topologies/mesh/elements/shape"] = "point";

  conduit::Node verify_info;
  if(!conduit::blueprint::mesh::verify(mesh,verify_info))
  {
    // verify failed, print error message
    CONDUIT_INFO("blueprint verify failed!" + verify_info.to_json());
  }
  

// Create an action that tells Ascent to:
//  add a scene (s1) with one plot (p1)
//  that will render a pseudocolor of 
//  the mesh field `rho`

  conduit::Node &add_act = actions.append(); 
  add_act["action"] = "add_scenes";

// declare a scene (s1) and pseudocolor plot (p1)
  conduit::Node &scenes = add_act["scenes"];
  scenes["s1/plots/p1/type"] = "pseudocolor";
  scenes["s1/plots/p1/field"] = "rho";
  scenes["s1/plots/p1/points/radius"] = .005;
  scenes["s1/plots/p1/points/radius_delta"] = .01;
  scenes["s1/renders/r1/image_prefix"] = "image.%05d";
  
  double vec3[3];
  vec3[0] = 0.04962330079677578; vec3[1] = -0.049605023484159; vec3[2] = 0.5;
  scenes["s1/renders/r1/camera/look_at"].set_float64_ptr(vec3,3);
  vec3[0] = 0.37579762254850335; vec3[1] = -4.8677403552214615; vec3[2] = 2.7920666007790262;
  scenes["s1/renders/r1/camera/position"].set_float64_ptr(vec3,3);
  vec3[0] = 0.20059944594166812; vec3[1] = 0.4318936019007353; vec3[2] = 0.8793337130606929;
  scenes["s1/renders/r1/camera/up"].set_float64_ptr(vec3,3);
  scenes["s1/renders/r1/camera/zoom"] = 2;
  /*
  scenes["s1/renders/r1/type"] = "cinema";
  scenes["s1/renders/r1/phi"] = 8;
  scenes["s1/renders/r1/theta"] = 8;
  scenes["s1/renders/r1/db_name"] = "example_db";
  */
  
  conduit::Node &extracts = add_act["extracts"];
  extracts["e1/type"]  = "relay";
  extracts["e1/params/path"] = "particles.%04d";
  extracts["e1/params/protocol"] = "blueprint/mesh/hdf5";
}

void Execute(sph::ParticlesData& sim)
{
  a.publish(mesh);
  a.execute(actions);
}

void Finalize()
{
  a.close();
}

}
#endif

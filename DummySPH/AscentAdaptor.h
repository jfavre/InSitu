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
  
  conduit::Node verify_info;
   
  std::cout << "time: " << sim.iteration*0.1 << " cycle: " << sim.iteration << std::endl;

  mesh["coordsets/coords/type"] = "explicit";
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

  mesh["topologies/mesh/coordset"] = "coords";

  mesh["fields/Density1/association"] = "vertex";
  mesh["fields/Density1/topology"] = "mesh";
  mesh["fields/Density1/volume_dependent"].set("false");
  
  mesh["fields/Density2/association"] = "vertex";
  mesh["fields/Density2/topology"] = "mesh";
  mesh["fields/Density2/volume_dependent"].set("false");
    
  mesh["fields/Density3/association"] = "vertex";
  mesh["fields/Density3/topology"] = "mesh";
  mesh["fields/Density3/volume_dependent"].set("false");
  
#ifdef STRIDED_SCALARS
  mesh["fields/Density1/values"].set_external_float64_ptr(&sim.scalars[0],
                                                     sim.n /* num_elements */,
                                                     0 * sizeof(conduit_float64) /* offset */,
                                                     sim.NbofScalarfields * sizeof(conduit_float64) /* stride*/,
                                                     sizeof(conduit_float64) /* element_bytes */,
                                                     0 /* endianness */);
  
  mesh["fields/Density2/values"].set_external_float64_ptr(&sim.scalars[0],
                                                     sim.n /* num_elements */,
                                                     1 * sizeof(conduit_float64) /* offset */,
                                                     sim.NbofScalarfields * sizeof(conduit_float64) /* stride*/,
                                                     sizeof(conduit_float64) /* element_bytes */,
                                                     0 /* endianness */);
  mesh["fields/Density3/values"].set_external_float64_ptr(&sim.scalars[0],
                                                     sim.n /* num_elements */,
                                                     2 * sizeof(conduit_float64) /* offset */,
                                                     sim.NbofScalarfields * sizeof(conduit_float64) /* stride*/,
                                                     sizeof(conduit_float64) /* element_bytes */,
                                                     0 /* endianness */);
#else
  mesh["fields/Density1/values"].set_external(sim.scalar1);
  mesh["fields/Density2/values"].set_external(sim.scalar2);
  mesh["fields/Density3/values"].set_external(sim.scalar3);
#endif


  mesh["fields/x/association"] = "vertex";
  mesh["fields/x/topology"]    = "mesh";
  mesh["fields/x/values"].set_external(sim.x);
  mesh["fields/x/volume_dependent"].set("false");
  mesh["fields/y/association"] = "vertex";
  mesh["fields/y/topology"]    = "mesh";
  mesh["fields/y/values"].set_external(sim.y);
  mesh["fields/y/volume_dependent"].set("false");
  mesh["fields/z/association"] = "vertex";
  mesh["fields/z/topology"]    = "mesh";
  mesh["fields/z/values"].set_external(sim.z);
  mesh["fields/z/volume_dependent"].set("false");

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
  scenes["s1/plots/p1/field"] = "Density1";
  scenes["s1/plots/p1/points/radius"] = .005;
  scenes["s1/plots/p1/points/radius_delta"] = .01;
  scenes["s1/renders/r1/image_prefix"] = "image.%05d";
  scenes["s1/renders/r1/annotations"] = "true";
  double vec3[3];
  vec3[0] = 0.0; vec3[1] = -0.0; vec3[2] = 0.0;
  scenes["s1/renders/r1/camera/look_at"].set_float64_ptr(vec3,3);
  vec3[0] = 0.0; vec3[1] = -0; vec3[2] = 10;
  scenes["s1/renders/r1/camera/position"].set_float64_ptr(vec3,3);
  vec3[0] = 0.0; vec3[1] = 1; vec3[2] = 0.0;
  scenes["s1/renders/r1/camera/up"].set_float64_ptr(vec3,3);
  scenes["s1/renders/r1/camera/zoom"] = 5;
  scenes["s1/renders/r1/image_width"] = 512;
  scenes["s1/renders/r1/image_height"] = 512;
  double dset_bounds[6] = {-1.0, 1.0, -1.0, 1.0, 0.0, 1.};
  scenes["s1/renders/r1/dataset_bounds"].set_float64_ptr(dset_bounds, 6);
   /*
  scenes["s1/renders/r1/type"] = "cinema";
  scenes["s1/renders/r1/phi"] = 8;
  scenes["s1/renders/r1/theta"] = 8;
  scenes["s1/renders/r1/db_name"] = "example_db";
  */
  

}

void Execute(sph::ParticlesData& sim)
{
  a.publish(mesh);
  a.execute(actions);
}

void Finalize()
{
  conduit::Node save_data_actions;
  conduit::Node &add_act = save_data_actions.append(); 
  add_act["action"] = "add_extracts";
  conduit::Node &extracts = add_act["extracts"];
  extracts["e1/type"] = "relay";
  extracts["e1/params/path"] = "mesh";
  extracts["e1/params/protocol"] = "blueprint/mesh/hdf5";

  a.publish(mesh);
  a.execute(save_data_actions);
  a.close();
}

}
#endif

#ifndef AscentAdaptor_h
#define AscentAdaptor_h

#include <lulesh.h>
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
  conduit::Node actions;
        
void Initialize()
{
  conduit::Node ascent_options;
  ascent_options["mpi_comm"] = MPI_Comm_c2f(MPI_COMM_WORLD);
  a.open(ascent_options);

// Create an action that tells Ascent to:
//  add a scene (s1) with one plot (p1)
//  that will render a pseudocolor of 
//  the mesh field `velocity`

  conduit::Node &add_scene = actions.append(); 
  add_scene["action"] = "add_scenes";

// declare a scene (s1) and pseudocolor plot (p1)
  conduit::Node &scenes = add_scene["scenes"];
  scenes["s1/plots/p1/type"] = "pseudocolor";
  scenes["s1/plots/p1/pipeline"] = "pl1";
  scenes["s1/plots/p1/field"] = "velocity";
  scenes["s1/renders/r1/image_prefix"] = "velocity.%05d";
}

void Execute(Domain& locDom)
{
  conduit::Node mesh ; // TODO = *locDom.node();
  a.publish(mesh);
  a.execute(actions);
}

void Finalize()
{
  a.close();
}

}
#endif

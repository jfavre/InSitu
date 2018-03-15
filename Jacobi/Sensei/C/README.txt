
I configure cmake with my install directory of my Sensei build. It would not
work with the build directory itself

I set up my regular VisIt environment

Running a 4-way parallel job (make && mpiexec -n 4 ./pjacobi)
using sim->m = 40; // mesh size (definition hidden in solvers.c)

I get a 43x43 grid displayed by VisIt. We split 40x40 into 4 sub-domains of 20x20
plus ghost points on each side I get 4 sub-domains of 22x22 which have a common
grid line (middle vertical and middle horizontal) and that makes a 43x43 grid

To use SENSEI 2.0 API add -DSENSEI_2=ON to configure line. This requires branch
request_specific_meshes.

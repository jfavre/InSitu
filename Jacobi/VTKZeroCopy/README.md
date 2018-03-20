# Zero Copy Demo
This illustrates use of zero copy in VTK Data Arrays.

## Compiling
```
cmake -DVTK_DIR=~/sc17/software/vtk/8.0.0/lib/
cmake/ ../
make
```

## AOS (Array of Structures)
```
./zero_copy aos 32 32
```

## SOA (Structure of Arrays)
```
./zero_copy soa 32 32
```

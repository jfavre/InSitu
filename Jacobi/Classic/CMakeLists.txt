CMAKE_MINIMUM_REQUIRED( VERSION 2.8)
PROJECT (PJACOBI C)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}")

FIND_PACKAGE(MPI REQUIRED) 
INCLUDE_DIRECTORIES(${MPI_C_INCLUDE_PATH})

SET(C_SRCS
solvers.c
pjacobi.c
)

ADD_EXECUTABLE(pjacobi ${C_SRCS})
TARGET_LINK_LIBRARIES(pjacobi ${MPI_C_LIBRARIES} m)
SET_PROPERTY(TARGET pjacobi APPEND PROPERTY COMPILE_FLAGS "-DPARALLEL")

OPTION(USE_LIBSIM "Use VisIt's libsim" OFF)
OPTION(USE_CATALYST "Use ParaView's Catalyst" OFF)

OPTION(INSITU_COUPLING "create an executable linked with in-situ library" ON)
if(INSITU_COUPLING)
  if(USE_LIBSIM)
    INCLUDE_DIRECTORIES("$ENV{VISITHOME}/current/linux-x86_64/libsim/V2/include")
    LINK_DIRECTORIES("$ENV{VISITHOME}/current/linux-x86_64/libsim/V2/lib")

    ADD_EXECUTABLE(pjacobi_visit ${C_SRCS} PJacobi_InSitu.c)
    TARGET_LINK_LIBRARIES(pjacobi_visit ${MPI_C_LIBRARIES} simV2.a dl m stdc++)
    SET_PROPERTY(TARGET pjacobi_visit APPEND PROPERTY COMPILE_FLAGS "-DUSE_LIBSIM -DPARALLEL")
  endif(USE_LIBSIM)

  if(USE_CATALYST)
    # we only need C++ compilers if we're building the Adaptor
    enable_language(CXX)
    find_package(ParaView 5.4 REQUIRED COMPONENTS vtkPVPythonCatalyst)
    include("${PARAVIEW_USE_FILE}")
    set(Adaptor_SRCS
      FEAdaptor.cxx
      )
    add_library(JacobiAdaptor ${Adaptor_SRCS})
    target_link_libraries(JacobiAdaptor vtkPVPythonCatalyst)
    if(NOT PARAVIEW_USE_MPI)
      message(SEND_ERROR "ParaView must be built with MPI enabled")
    endif()

  add_executable(pjacobi_catalyst pjacobi.c solvers.c)
  set_property(TARGET pjacobi_catalyst APPEND PROPERTY COMPILE_FLAGS "-DUSE_CATALYST -DPARALLEL")

  target_link_libraries(pjacobi_catalyst LINK_PRIVATE ${MPI_LIBRARIES} JacobiAdaptor)
  include(vtkModuleMacros)
  include(vtkMPI)
  vtk_mpi_link(pjacobi_catalyst)
endif(USE_CATALYST)

endif(INSITU_COUPLING)


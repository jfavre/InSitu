CMAKE_MINIMUM_REQUIRED( VERSION 2.8)
PROJECT (PJACOBI)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}")

OPTION(INSITU_COUPLING "create an executable using SENSEI" ON)
if(INSITU_COUPLING)
  # make use of sense 2.0 API
  # see branch request_specific_meshes
  OPTION(SENSEI_2 "use SENSEI 2.0 API" OFF)
  # compile the exercise or the solution
  OPTION(MAKE_SOLUTION "compile the solution" OFF)
  # find sensei
  find_package(SENSEI REQUIRED)
endif()



FIND_PACKAGE(MPI REQUIRED)
INCLUDE_DIRECTORIES(${MPI_C_INCLUDE_PATH})

set(sources solvers.c pjacobi.c)
set(libs m)

if(ENABLE_SENSEI)
  list(APPEND sources Bridge.cxx)
  list(APPEND libs sensei)
  if (MAKE_SOLUTION)
    set(DATA_ADAPTOR_DIR "${PROJECT_SOURCE_DIR}/solution")
  else()
    set(DATA_ADAPTOR_DIR "${PROJECT_SOURCE_DIR}/exercise")
  endif()
  include_directories(${DATA_ADAPTOR_DIR})
  list(APPEND sources ${DATA_ADAPTOR_DIR}/JacobiDataAdaptor.cxx)
  add_definitions("-DENABLE_SENSEI")
  if (SENSEI_2)
    add_definitions("-DSENSEI_2")
  endif()
endif()

ADD_EXECUTABLE(pjacobi ${sources})
TARGET_LINK_LIBRARIES(pjacobi ${MPI_C_LIBRARIES} ${libs})
SET_PROPERTY(TARGET pjacobi APPEND PROPERTY COMPILE_FLAGS "-DPARALLEL")

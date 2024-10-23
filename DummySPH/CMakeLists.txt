cmake_minimum_required(VERSION 3.13)
project (DummySPH C CXX)

include (GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

# since we use C++11 in this example.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Since this example uses MPI, find and link against it.
find_package(MPI COMPONENTS C CXX)
if (NOT MPI_FOUND)
  message(STATUS
    "Skipping example: ${CMAKE_PROJECT_NAME} requires MPI.")
  return ()
endif ()

set(C_SRCS
solvers.cxx
Driver.cxx
)

add_executable(dummysph ${C_SRCS})
target_link_libraries(dummysph PRIVATE MPI::MPI_C MPI::MPI_CXX m)

option(STRIDED_SCALARS "Use interleaved scalar arrays") 

set(INSITU None CACHE STRING "Enable in-situ support")
set_property(CACHE INSITU PROPERTY STRINGS None Catalyst Ascent)

if(INSITU STREQUAL "Catalyst")
  find_package(catalyst REQUIRED PATHS "/local/apps/ParaView/")
  # we only need C++ compilers if we're building the Adaptor
  enable_language(CXX)

  add_executable(dummysph_catalystV2 ${C_SRCS})
  target_compile_definitions(dummysph_catalystV2 PRIVATE USE_CATALYST=1)
if(STRIDED_SCALARS)
  target_compile_definitions(dummysph_catalystV2 PRIVATE STRIDED_SCALARS=1)
endif()
  target_link_libraries(dummysph_catalystV2 PRIVATE MPI::MPI_C MPI::MPI_CXX m catalyst::catalyst)
elseif(INSITU STREQUAL "Ascent")
  find_package(Ascent REQUIRED PATHS "/local/apps/ascent/install/lib/cmake/ascent")

  add_executable(dummysph_ascent ${C_SRCS})
  target_compile_definitions(dummysph_ascent PRIVATE USE_ASCENT=1)
  if(STRIDED_SCALARS)
  target_compile_definitions(dummysph_ascent PRIVATE STRIDED_SCALARS=1)
endif()
  target_link_libraries(dummysph_ascent MPI::MPI_C MPI::MPI_CXX ascent::ascent)
endif()


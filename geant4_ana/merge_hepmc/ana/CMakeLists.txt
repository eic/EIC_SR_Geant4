#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(EIC)

#----------------------------------------------------------------------------
# Setup include directory for this project
#
EXECUTE_PROCESS(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
#----------------------------------------------------------------------------
# Find ROOT (optional package)
#
find_package(ROOT REQUIRED)
if(ROOT_FOUND)
  add_definitions(-DG4ANALYSIS_USE_ROOT)
endif()

#----------------------------------------------------------------------------
# Find HEPMC3
find_package(HepMC3 REQUIRED)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${ROOT_INCLUDE_DIR}
                    ${HEPMC3_INCLUDE_DIR})
                    
#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(exe main.cc)

EXECUTE_PROCESS(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_EXE_LINKER_FLAGS ${ROOT_LD_FLAGS})

target_link_libraries(exe ${ROOT_LIBRARIES} ${HEPMC3_LIB})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(exe_SCRIPTS)

foreach(_script ${exe_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS exe DESTINATION bin)

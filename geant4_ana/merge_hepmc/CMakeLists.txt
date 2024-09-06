cmake_minimum_required (VERSION 3.5 FATAL_ERROR)
project (EIC_ANA C CXX)

#----------------------------------------------------------------------------
# Find ROOT and HEPMC3
find_package(ROOT 6.24 REQUIRED)
find_package(HepMC3 3.2 REQUIRED)

#----------------------------------------------------------------------------
# Compile definitions (see https://cmake.org/cmake/help/v3.0/command/execute_process.html)
execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS ${ROOT_LD_FLAGS})

#----------------------------------------------------------------------------
# Locate sources and headers for this project (see https://cmake.org/cmake/help/latest/command/include_directories.html)
include_directories(${ROOT_INCLUDE_DIR} ${HEPMC3_INCLUDE_DIR})
                    
#----------------------------------------------------------------------------
# Add the executable, and link it to ROOT and HEPMC3 libraries (see https://cmake.org/cmake/help/latest/command/add_executable.html)
add_executable(exe merge_hepmc.cc)
target_link_libraries(exe ${ROOT_LIBRARIES} ${HEPMC3_LIB})

#----------------------------------------------------------------------------
# Install the executable (see https://cmake.org/cmake/help/latest/command/install.html)
install(TARGETS exe)

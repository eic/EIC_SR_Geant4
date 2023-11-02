cmake_minimum_required (VERSION 3.5 FATAL_ERROR)
project (EIC_ANA C CXX)

#----------------------------------------------------------------------------
# Find ROOT
find_package(ROOT REQUIRED)

#----------------------------------------------------------------------------
# Compile definitions
EXECUTE_PROCESS(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
EXECUTE_PROCESS(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS ${ROOT_LD_FLAGS})

#----------------------------------------------------------------------------
# Locate sources and headers for this project 
include_directories(${PROJECT_SOURCE_DIR}/src 
                    ${ROOT_INCLUDE_DIR})
                    
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to ROOT libraries 
add_executable(exe ana.cc ${sources} ${headers})
target_link_libraries(exe ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to './' directory under CMAKE_INSTALL_PREFIX
install(TARGETS exe DESTINATION ./)

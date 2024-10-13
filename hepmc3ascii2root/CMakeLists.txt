cmake_minimum_required(VERSION 3.14)

project(hepmc3ascii2root)

find_package(HepMC3 REQUIRED)

add_executable(hepmc3ascii2root hepmc3ascii2root ${CMAKE_CURRENT_SOURCE_DIR}/hepmc3ascii2root.cc)
target_include_directories(hepmc3ascii2root PRIVATE ${HEPMC3_INCLUDE_DIR})
target_link_libraries(hepmc3ascii2root PRIVATE ${HEPMC3_LIBRARIES})

install(TARGETS hepmc3ascii2root DESTINATION ${CMAKE_INSTALL_BINDIR})

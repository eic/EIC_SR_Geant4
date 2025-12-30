cmake_minimum_required(VERSION 3.30)
project(SteppingPlugin LANGUAGES CXX)

find_package(DD4hep 1.18 REQUIRED COMPONENTS DDCore DDG4)

dd4hep_add_plugin(MySteppingAction
    SOURCES *.cxx
    USES    DD4hep::DDG4 
    )
install(TARGETS MySteppingAction LIBRARY DESTINATION lib)

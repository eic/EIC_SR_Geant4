#!/bin/bash
source /home/natochii/eic/epic/install/bin/thisepic.sh 
cmake -B build -S . -DCMAKE_CXX_COMPILER=g++ -DCMAKE_INSTALL_PREFIX=install
cmake --build build -j10 -- install

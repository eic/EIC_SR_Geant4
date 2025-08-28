#!/bin/bash
source /gpfs02/eic/anatochii/eic/epic/install/bin/thisepic.sh 
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=install
cmake --build build -j10 -- install

#!/bin/bash
source /home/natochii/eic/epic/install/bin/thisepic.sh 
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=install
cmake --build build -j10 -- install

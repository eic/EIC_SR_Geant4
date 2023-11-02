#!/bin/bash

# output directory
mkdir -pv output

# build directory
mkdir -pv build
cd build

# clean
make clean

# config
cmake ../

# compile
make -j4

# run
#mainInDir="../../geant4_sim/output_5keVcut"
#outputFileName="output_5keVcut"

mainInDir="../../geant4_sim/output_noEcut"
outputFileName="output_noEcut"

inputFileName="output_*.root"
mainOutDir="../output"
treeName="tree"

./exe ${mainInDir} ${inputFileName} ${mainOutDir} ${outputFileName} ${treeName} 

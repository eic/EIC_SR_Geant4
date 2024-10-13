#!/bin/bash

inDirName='/Users/andriinatochii/Desktop/HighStatTracks/SynradG4_v6.3.1_20240906_18x275GeV_HepMCfiles_IPphotons'
inFileName='output_g4ana_v6.3.1_20240906_18x275GeV_'
inFileNum=100
outDirName='/Users/andriinatochii/Desktop/HighStatTracks/gen_hepmc/output'

mkdir -pv ${outDirName}
mkdir -pv build
cd build

cmake ../
make -j4

time ./exe ${inDirName} ${inFileName} ${inFileNum} ${outDirName}

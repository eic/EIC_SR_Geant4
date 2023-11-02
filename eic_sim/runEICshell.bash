#!/bin/bash

# load geometry
source /opt/detector/setup.sh

# output dir
mkdir -pv output

# file name
inputFileName='../geant4_ana/output/output_5keVcut.hepmc'
outputFileName='./output/output_5keVcut.edm4hep.root'

# run for all (-1) events in the input file
time npsim --runType batch --numberOfEvents -1 --compactFile ${DETECTOR_PATH}/${DETECTOR_CONFIG}.xml --inputFiles ${inputFileName} --outputFile ${outputFileName}

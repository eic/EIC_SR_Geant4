#!/bin/bash


# load geometry
source /opt/detector/setup.sh

rm -rf build
mkdir -pv build
cd build

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
inputDirName='../data'
inputFileName='output.hepmc'
outputDirName='../output'
outputFileName='output.edm4hep.root'
logFileName='log.edm4hep.txt'
#--------------------------------------------------#
#--------------------------------------------------#

echo "[INFO] inputDirName = ${inputDirName}"
echo "[INFO] inputFileName = ${inputFileName}"
echo "[INFO] outputDirName = ${outputDirName}"
echo "[INFO] outputFileName = ${outputFileName}"
echo "[INFO] logFileName = ${logFileName}"

# run for all (-1) events in the input file
time npsim --runType batch --numberOfEvents -1 --compactFile ${DETECTOR_PATH}/${DETECTOR_CONFIG}.xml --inputFiles ${inputDirName}/${inputFileName} --outputFile ${outputDirName}/${outputFileName} &> ${outputDirName}/${logFileName}
cd -

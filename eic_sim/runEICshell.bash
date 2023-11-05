#!/bin/bash


# load geometry
source /opt/detector/setup.sh

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
dirName='data'
inputFileName='output.hepmc'
outputFileName='output.edm4hep.root'
logFileName='log.edm4hep.txt'
#--------------------------------------------------#
#--------------------------------------------------#

echo "[INFO] dirName = ${dirName}"
echo "[INFO] inputFileName = ${inputFileName}"
echo "[INFO] outputFileName = ${outputFileName}"
echo "[INFO] logFileName = ${logFileName}"

# run for all (-1) events in the input file
#time npsim --runType batch --numberOfEvents -1 --compactFile ${DETECTOR_PATH}/${DETECTOR_CONFIG}.xml --inputFiles ${dirName}/${inputFileName} --outputFile ${dirName}/${outputFileName} &> ${dirName}/${logFileName}

# run for the specified number of events in the input file
time npsim --runType batch --numberOfEvents 1000 --compactFile ${DETECTOR_PATH}/${DETECTOR_CONFIG}.xml --inputFiles ${dirName}/${inputFileName} --outputFile ${dirName}/${outputFileName} &> ${dirName}/${logFileName}

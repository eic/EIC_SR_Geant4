#!/bin/bash

# get input parameters
jobID=$1
nParticles=$2
xmlFile=$3
seed=$4
outputDir=$5

mkdir -pv ${outputDir}

date > ${outputDir}/logExe_${jobID}.txt

# create mac file directory if it was not done before
mkdir -pv macFiles

# create a mac file
echo "##-----------------------------------------##"		> ./macFiles/run_${jobID}.mac
echo "/run/verbose 1"						>> ./macFiles/run_${jobID}.mac
echo "/event/verbose 0"						>> ./macFiles/run_${jobID}.mac
echo "/tracking/verbose 0"					>> ./macFiles/run_${jobID}.mac
echo "##-----------------------------------------##"		>> ./macFiles/run_${jobID}.mac
echo "/output/filename ${outputDir}/output_${jobID}.root"	>> ./macFiles/run_${jobID}.mac
echo ""								>> ./macFiles/run_${jobID}.mac
echo "/stack/killsecondaries 0"					>> ./macFiles/run_${jobID}.mac
echo ""								>> ./macFiles/run_${jobID}.mac
echo "/run/initialize"						>> ./macFiles/run_${jobID}.mac
echo "##-----------------------------------------##"		>> ./macFiles/run_${jobID}.mac
echo "/run/beamOn ${nParticles}"				>> ./macFiles/run_${jobID}.mac
echo "##-----------------------------------------##"		>> ./macFiles/run_${jobID}.mac 

# dump the mac file content to the log file
cat ./macFiles/run_${jobID}.mac >> ${outputDir}/logExe_${jobID}.txt

# run the simulation
./exe ./macFiles/run_${jobID}.mac ${xmlFile} ${seed} >> ${outputDir}/logExe_${jobID}.txt

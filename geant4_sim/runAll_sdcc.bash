#!/bin/bash

# get input parameters
jobID=$1
nParticles=$2
xmlFile=$3
eicshell=$4
outputDir=$5

seed=$RANDOM

mkdir -pv ${outputDir}

date > ${outputDir}/logExe_${jobID}.txt

# create mac file directory if it was not done before
mkdir -pv macFiles
mkdir -pv exeFiles

# create a mac file
echo "##-----------------------------------------##"		> ./macFiles/run_${jobID}.mac
echo "/run/verbose 1"						>> ./macFiles/run_${jobID}.mac
echo "/event/verbose 0"						>> ./macFiles/run_${jobID}.mac
echo "/tracking/verbose 0"					>> ./macFiles/run_${jobID}.mac
echo "##-----------------------------------------##"		>> ./macFiles/run_${jobID}.mac
echo "/output/filename ${outputDir}/output_${jobID}.root"	>> ./macFiles/run_${jobID}.mac
echo ""								>> ./macFiles/run_${jobID}.mac
echo "/stack/killsecondaries 0"					>> ./macFiles/run_${jobID}.mac
echo "/detector/dist_tag 8.6  m"                                >> ./macFiles/run_${jobID}.mac
echo "/detector/dist_tag 16.6 m"                                >> ./macFiles/run_${jobID}.mac
echo "/detector/dist_tag 24.7 m"                                >> ./macFiles/run_${jobID}.mac
echo "/detector/dist_tag 29.2 m"                                >> ./macFiles/run_${jobID}.mac
echo "/detector/dist_tag 31.3 m"                                >> ./macFiles/run_${jobID}.mac
echo "/detector/bwd_achmbH 10 cm"                               >> ./macFiles/run_${jobID}.mac
echo "/detector/ModifyGeometry"                                 >> ./macFiles/run_${jobID}.mac
echo ""								>> ./macFiles/run_${jobID}.mac
echo "/run/initialize"						>> ./macFiles/run_${jobID}.mac
echo "##-----------------------------------------##"		>> ./macFiles/run_${jobID}.mac
echo "/run/beamOn ${nParticles}"				>> ./macFiles/run_${jobID}.mac
echo "##-----------------------------------------##"		>> ./macFiles/run_${jobID}.mac 

# dump the mac file content to the log file
cat ./macFiles/run_${jobID}.mac >> ${outputDir}/logExe_${jobID}.txt

# run the simulation
echo "./exe ./macFiles/run_${jobID}.mac ${xmlFile} ${seed} >> ${outputDir}/logExe_${jobID}.txt" > ./exeFiles/exe_${jobID}.bash
chmod 777 ./exeFiles/exe_${jobID}.bash
. ${eicshell} -- ./exeFiles/exe_${jobID}.bash

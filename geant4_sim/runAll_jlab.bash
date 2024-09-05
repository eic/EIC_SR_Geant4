#!/bin/bash

# get input parameters
taskID=$1
nParticles=$2
xmlFile=$3
eicshell=$4
outputDir=$5

seed=$RANDOM

date

# create mac file directory if it was not done before
mkdir -pv macFiles
mkdir -pv exeFiles

# create a mac file
echo "##-----------------------------------------##"		> ./macFiles/run_${taskID}.mac
echo "/run/verbose 1"						>> ./macFiles/run_${taskID}.mac
echo "/event/verbose 0"						>> ./macFiles/run_${taskID}.mac
echo "/tracking/verbose 0"					>> ./macFiles/run_${taskID}.mac
echo "##-----------------------------------------##"		>> ./macFiles/run_${taskID}.mac
echo "/output/filename ${outputDir}/output_${taskID}.root"	>> ./macFiles/run_${taskID}.mac
echo ""								>> ./macFiles/run_${taskID}.mac
echo "/stack/killsecondaries 0"					>> ./macFiles/run_${taskID}.mac
echo "/detector/dist_tag 8.6  m"                                >> ./macFiles/run_${taskID}.mac
echo "/detector/dist_tag 16.6 m"                                >> ./macFiles/run_${taskID}.mac
echo "/detector/dist_tag 24.7 m"                                >> ./macFiles/run_${taskID}.mac
echo "/detector/dist_tag 29.2 m"                                >> ./macFiles/run_${taskID}.mac
echo "/detector/dist_tag 31.3 m"                                >> ./macFiles/run_${taskID}.mac
echo "/detector/bwd_achmbH 10 cm"                               >> ./macFiles/run_${taskID}.mac
echo "/detector/ModifyGeometry"                                 >> ./macFiles/run_${taskID}.mac
echo ""								>> ./macFiles/run_${taskID}.mac
echo "/run/initialize"						>> ./macFiles/run_${taskID}.mac
echo "##-----------------------------------------##"		>> ./macFiles/run_${taskID}.mac
echo "/run/beamOn ${nParticles}"				>> ./macFiles/run_${taskID}.mac
echo "##-----------------------------------------##"		>> ./macFiles/run_${taskID}.mac 

# dump the mac file content to the log file
cat ./macFiles/run_${taskID}.mac

# run the simulation
echo "./exe ./macFiles/run_${taskID}.mac ${xmlFile} ${seed}" > ./exeFiles/exe_${taskID}.bash
chmod 777 ./exeFiles/exe_${taskID}.bash
. ${eicshell} -- ./exeFiles/exe_${taskID}.bash

#!/bin/bash

# get input parameters
jobID=$1
nParticles=$2
xmlFile=$3
seed=$4

date > ../output/log_${jobID}.txt

# create mac file directory if it was not done before
mkdir -pv macFiles

# create a mac file
echo "##-----------------------------------------##"		> ./macFiles/run_${jobID}.mac
echo "/run/verbose 1"						>> ./macFiles/run_${jobID}.mac
echo "/event/verbose 0"						>> ./macFiles/run_${jobID}.mac
echo "/tracking/verbose 0"					>> ./macFiles/run_${jobID}.mac
echo "##-----------------------------------------##"		>> ./macFiles/run_${jobID}.mac
echo "/output/filename ../output/output_${jobID}.root"		>> ./macFiles/run_${jobID}.mac
echo ""								>> ./macFiles/run_${jobID}.mac
echo "/stack/killsecondaries 0"					>> ./macFiles/run_${jobID}.mac
echo ""								>> ./macFiles/run_${jobID}.mac
echo "/run/initialize"						>> ./macFiles/run_${jobID}.mac
echo "##-----------------------------------------##"		>> ./macFiles/run_${jobID}.mac
echo "/run/beamOn ${nParticles}"				>> ./macFiles/run_${jobID}.mac
echo "##-----------------------------------------##"		>> ./macFiles/run_${jobID}.mac 

# load env. variables
# geat4
. /usr/local/Cellar/spack/0.20.1/share/spack/setup-env.sh; spack load geant4
# root
. /usr/local/bin/thisroot.sh

# dump the mac file content to the log file
cat ./macFiles/run_${jobID}.mac >> ../output/log_${jobID}.txt

# run the simulation
./exe ./macFiles/run_${jobID}.mac ${xmlFile} ${seed} >> ../output/log_${jobID}.txt

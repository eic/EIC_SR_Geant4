#!/bin/bash
LANG=en_US
LC_NUMERIC=en_US.UTF-8

date;

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
xmlFile="${PWD}/geometry/simParameters.xml" # simulation geometry settings
nParticles=1000 # number of primary particles per job
outputDir="${PWD}/output" # output directory
nJobs=10
#--------------------------------------------------#
#--------------------------------------------------#

echo ""
echo "[INFO] Input parameters:"
echo " xmlFile = ${xmlFile}"
echo " nParticles = ${nParticles}"
echo " nJobs = ${nJobs}"
echo " outputDir = ${outputDir}"
echo ""

# output directory
#if [ -d "${outputDir}" ]
#then 
#	dirName=${outputDir}_$(date +%s)
#	mv ${outputDir} ${dirName}
#	echo "[INFO] Directory '${outputDir}' exists. Moved to '${dirName}'"; 
#fi
mkdir -pv ${outputDir}

# build directory
mkdir -pv ./build
cd ./build/

# config
cmake ../

# compile
make -j4

chmod 777 runAll.bash;

mkdir -pv ./macFiles

for jobID in $(seq 1 1 ${nJobs})
do
        echo "[INFO] GNU Screen session name: jobID_$jobID";

	# submit a session
        screen -S jobID_$jobID -d -m ./runAll.bash $jobID $nParticles $xmlFile $RANDOM ${outputDir}
done
screen -ls

cd -

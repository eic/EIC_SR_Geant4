#!/bin/bash
LANG=en_US
LC_NUMERIC=en_US.UTF-8

date;

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
xmlFile='../geometry/simParameters.xml'
nParticles=100
nJobs=1
#--------------------------------------------------#
#--------------------------------------------------#

mkdir -pv ./build
cd ./build/

g4conf
g4cmake
make -j4

echo -ne "[INFO] Total number of threads on the machine is: ";
sysctl -n hw.ncpu

chmod 777 runAll.bash;

mkdir -pv macFiles
mkdir -pv logFiles

for jobID in $(seq 1 1 ${nJobs})
do
        echo "[INFO] GNU Screen session name: jobID_$jobID";

        screen -S jobID_$jobID -d -m ./runAll.bash $jobID $nParticles $xmlFile
done
screen -ls

cd -

#!/bin/bash
LANG=en_US
LC_NUMERIC=en_US.UTF-8

date;

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
xmlFile='../geometry/simParameters.xml'
nParticles=10000 # personal macbook with 6 cores (x2 threads): 0.35 sec/event for 10 jobs
nJobs=10

echo "\n[INFO] Input parameters"
echo "xmlFile = ${xmlFile}"
echo "nParticles = ${nParticles}"
echo "nJobs = ${nJobs}\n"
#--------------------------------------------------#
#--------------------------------------------------#

# output directory
if [ -d "./output/" ]
then 
	dirName=./output_$(date +%s)
	mv ./output ${dirName}
	echo "[INFO] Directory './output/' exists. Moved to '${dirName}'"; 
fi
mkdir -pv ./output

# build directory
mkdir -pv ./build
cd ./build/

# load geant4 from spack
. /usr/local/Cellar/spack/0.20.1/share/spack/setup-env.sh
spack load geant4

# config
cmake ../

# compile
make -j4

echo -ne "[INFO] Total number of threads on the machine is: ";
sysctl -n hw.ncpu

chmod 777 runAll.bash;

mkdir -pv ./macFiles

for jobID in $(seq 1 1 ${nJobs})
do
        echo "[INFO] GNU Screen session name: jobID_$jobID";

        screen -S jobID_$jobID -d -m ./runAll.bash $jobID $nParticles $xmlFile $RANDOM
done
screen -ls

cd -

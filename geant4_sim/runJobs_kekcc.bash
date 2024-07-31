#!/bin/bash
LANG=en_US
LC_NUMERIC=en_US.UTF-8

date;

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
xmlFile="${PWD}/geometry/simParameters_v6.3.xml" # simulation geometry settings
nParticles=1000 # number of primary particles per job
outputDir='/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_srmask5_extendedBWD_stdFWD_Q3ERorbitCheck' # output directory # default radius
buildDir='./build' # build directory
nJobs=1000 # number of jobs
queue='s'
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
if [ -d "${outputDir}" ]
then 
	dirName=${outputDir}_$(date +%s)
	mv ${outputDir} ${dirName}
	echo "[INFO] Directory '${outputDir}' exists. Moved to '${dirName}'"; 
fi
mkdir -pv ${outputDir}

# build directory
mkdir -pv ${buildDir}
cd ${buildDir}

# config
make clean
cmake ../

# compile
make -j4

chmod 777 runAll.bash;

mkdir -pv ./macFiles

for jobID in $(seq 1 1 ${nJobs})
do
	# count how many jobs are pending, the maximum is 20000 jobs
	nPend=$(bjobs | grep "PEND" | wc -l);
	while [ $nPend -gt 19000 ]
	do 
		date
		echo "[INFO] Waiting 10min for an empty slot to submit the next job to an LSF queue ..."
		sleep 600
		nPend=$(bjobs | grep "PEND" | wc -l)
	done
	
	# submit a job
	com="./runAll.bash $jobID $nParticles $xmlFile $RANDOM ${outputDir}"
	echo -ne "[INFO] ${com} => "
	bsub -q $queue -o ${outputDir}/logJob_${jobID}.txt "${com}"
done

cd -

#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
mainInDir='/group/belle2/TMP/anatochi/eic/output' # input directory
treeName='tree'
outputDir='/group/belle2/TMP/anatochi/eic/output_ana' # output directory
buildDir='./build' # build directory
nJobs=1000 # number of jobs
queue='s'
#--------------------------------------------------#
#--------------------------------------------------#

# output directory
mkdir -pv ${outputDir}

# build directory
mkdir -pv ${buildDir}
cd ${buildDir}

# config
cmake ../

# compile
make -j4

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
	com="./exe ${mainInDir} output_${jobID}.root ${outputDir} output_ana_${jobID} ${treeName} &> ${outputDir}/logExe_ana_${jobID}.txt"
	echo -ne "[INFO] ${com} => "
	bsub -q $queue -o ${outputDir}/logJob_ana_${jobID}.txt "${com}"
done

cd -

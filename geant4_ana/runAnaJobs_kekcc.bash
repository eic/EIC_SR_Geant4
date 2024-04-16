#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
mainInDir='/group/belle2/TMP/anatochi/eic/highstat_output_g4sim_v6.2_srmask5_1_ip_cut' # input directory
treeName='tree'
outputDir='/group/belle2/TMP/anatochi/eic/highstat_output_g4ana_v6.2_srmask5_1_ip_cut' # output directory
buildDir='./build' # build directory
nJobs=10000 # number of jobs
queue='l'
# in the custom-built Geant4 coordinate system
minZ=-500.0 # [cm] minimum Z
maxZ=4000.0 # [cm] maximum Z
#--------------------------------------------------#
#--------------------------------------------------#

echo "[INFO] mainInDir = ${mainInDir}"
echo "[INFO] treeName = ${treeName}"
echo "[INFO] outputDir = ${outputDir}"
echo "[INFO] buildDir = ${buildDir}"
echo "[INFO] nJobs = ${nJobs}"
echo "[INFO] queue = ${queue}"
echo "[INFO] minZ = ${minZ} [cm]"
echo "[INFO] maxZ = ${maxZ} [cm]"

# output directory
mkdir -pv ${outputDir}

# build directory
rm -rf ${buildDir}
mkdir -pv ${buildDir}
cd ${buildDir}

# config
make clean
cmake ../

# compile
make -j4

runID=0
for jobID in $(seq 1 1 ${nJobs})
do
	# submit a job
	com="./exe ${mainInDir} output_${jobID}.root ${outputDir} output_ana_${jobID} ${treeName} ${minZ} ${maxZ}"
	com="${com} &> ${outputDir}/logExe_ana_${jobID}.txt"
	echo "$com" >> run_${runID}.sh
	jobID_tmp=$(($jobID % 10))
	if [ $jobID_tmp -eq 0 ]; then
		echo -ne "Run = ${runID} (last job = ${jobID}/${nJobs}): "
		chmod a+x run_${runID}.sh
		bsub -q $queue -o ${outputDir}/logRun_ana_${runID}.txt "./run_${runID}.sh"
		runID=$(($runID+1))
	fi	
done

cd -

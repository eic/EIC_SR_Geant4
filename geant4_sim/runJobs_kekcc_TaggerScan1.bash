#!/bin/bash
LANG=en_US
LC_NUMERIC=en_US.UTF-8

date;

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
xmlFile="${PWD}/geometry/simParameters_v6.3.xml" # simulation geometry settings
nParticles=100000 # number of primary particles per job
outputDir='/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_TaggerScan1_ChamberH_10cm' # output directory # default radius
buildDir='./build1' # build directory
queue='s'
#--------------------------------------------------#
#--------------------------------------------------#

echo ""
echo "[INFO] Input parameters:"
echo " xmlFile = ${xmlFile}"
echo " nParticles = ${nParticles}"
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

chmod 777 runAll_TaggerScan1.bash;

mkdir -pv ./macFiles

# Scan tagger2 position along the ante-chamber
for PosZ in {-995..995..5}
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
	com="./runAll_TaggerScan1.bash $PosZ $nParticles $xmlFile $RANDOM ${outputDir} ${PosZ}"
	echo -ne "[INFO] ${com} => "
	bsub -q $queue -o ${outputDir}/logJob_${PosZ}.txt "${com}"
done

cd -

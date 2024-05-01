#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
inputDirName='/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_g4ana_v6.2_srmask5_1_ip_cut3' # input directory
outputDirName='/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_eicsim_v6.2_srmask5_1_ip_cut4_2' # output directory
buildDir='./build4_2' # build directory
nJobs=10000 # number of jobs
queue='s'
eicshell='/home/belle2/anatochi/eic/eic-shell'
#--------------------------------------------------#
#--------------------------------------------------#

echo "[INFO] inputDirName = ${inputDirName}"
echo "[INFO] outputDirName = ${outputDirName}"
echo "[INFO] buildDir = ${buildDir}"
echo "[INFO] nJobs = ${nJobs}"
echo "[INFO] queue = ${queue}"

# output directory
mkdir -pv ${outputDirName}
# build directory
rm -rf ${buildDir}
mkdir -pv ${buildDir}
cd ${buildDir}

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
 	runDir="runEICsim_${jobID}"
 	mkdir -pv ${runDir}
 	cd ${runDir}
 	runFile="runEICsim.sh"
 	echo "#!/bin/bash" > ${runFile}

 	# echo "source /opt/detector/setup.sh" >> ${runFile}
	echo "source /home/belle2/anatochi/eic/epic/install/setup.sh" >> ${runFile}

 	# echo -ne "time npsim --runType batch --numberOfEvents -1 --compactFile /opt/detector/epic-nightly/share/epic/epic.xml" >> ${runFile}
	echo -ne "time npsim --runType batch --numberOfEvents -1 --compactFile /home/belle2/anatochi/eic/epic/install/share/epic/epic.xml" >> ${runFile}

# 	echo " --inputFiles ${inputDirName}/output_ana_${jobID}.hepmc --outputFile ${outputDirName}/output_eicsim_${jobID}.edm4hep.root &> ${outputDirName}/logExe_eicsim_${jobID}.txt" >> ${runFile}

 	echo " --inputFiles ${inputDirName}/output_ana_${jobID}.hepmc --outputFile ${outputDirName}/output_eicsim_${jobID}.edm4hep.root" >> ${runFile}

 	chmod 777 ${runFile}
# 	bsub -q $queue -o ${outputDirName}/logJob_eicsim_${jobID}.txt ". ${eicshell} -- ./${runFile}"
 	bsub -q $queue -o /dev/null ". ${eicshell} -- ./${runFile}"
 	cd -
done

cd -
date

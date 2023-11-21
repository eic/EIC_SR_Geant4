#!/bin/bash


#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
inputDirName='/gpfs/group/belle2/TMP/anatochi/eic/output_ana' # input directory
outputDirName='/gpfs/group/belle2/TMP/anatochi/eic/output_ddsim' # output directory
buildDir='./build' # build directory
nJobs=1000 # number of jobs
queue='l'
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
	runFile="runDDSIM_${jobID}.sh"
	echo "#!/bin/bash" > ${runFile}
	echo "source /opt/detector/epic-nightly/setup.sh" >> ${runFile}
	echo "time npsim --runType batch --numberOfEvents -1 --compactFile /opt/detector/epic-nightly/share/epic/epic.xml \
--inputFiles ${inputDirName}/output_ana_${jobID}.hepmc --outputFile ${outputDirName}/output_ddsim_${jobID}.edm4hep.root" >> ${runFile}

	chmod 777 ${runFile}
	bsub -q $queue -o ${outputDirName}/logJob_ddsim_${jobID}.txt ". ${eicshell} -- ./${runFile}"
done

cd -

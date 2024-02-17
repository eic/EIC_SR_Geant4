#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
inputDirName='/gpfs/group/belle2/TMP/anatochi/eic/output_g4ana_nosrmask_ultrahighstat' # input directory
outputDirName='/gpfs/group/belle2/TMP/anatochi/eic/output_eicsim_nosrmask_ultrahighstat' # output directory
buildDir='./build' # build directory
nJobs=100000 # number of jobs
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
rm -rf ${buildDir}
mkdir -pv ${buildDir}
cd ${buildDir}

runFile="runEICsim.sh"

runID=0
runDir="runEICsim_${runID}"
mkdir -pv ${runDir}
cd ${runDir}
echo "#!/bin/bash" > ${runFile}
echo "source /home/belle2/anatochi/eic/epic/install/setup.sh" >> ${runFile}
# echo "source /opt/detector/setup.sh" >> ${runFile}

# loop over all jobs
for jobID in $(seq 1 1 ${nJobs})
do
	# echo -ne "time npsim --runType batch --numberOfEvents -1 --compactFile /opt/detector/epic-nightly/share/epic/epic.xml" >> ${runFile}
	echo -ne "time npsim --runType batch --numberOfEvents -1 --compactFile /home/belle2/anatochi/eic/epic/install/share/epic/epic.xml" >> ${runFile}
	echo " --inputFiles ${inputDirName}/output_ana_${jobID}.hepmc --outputFile ${outputDirName}/output_eicsim_${jobID}.edm4hep.root &> ${outputDirName}/logExe_eicsim_${jobID}.txt" >> ${runFile}

	jobID_tmp=$(($jobID % 100))
	if [ $jobID_tmp -eq 0 ]; then
		# make the file executable
		chmod 777 ${runFile}
		
		# submit a job
		echo -ne "Run = ${runID} (last job = ${jobID}/${nJobs}): "
		bsub -q $queue -o ${outputDirName}/logJob_eicsim_${jobID}.txt ". ${eicshell} -- ./${runFile}"
		cd -
	
		# move to another run
		runID=$(($runID+1))
		runDir="runEICsim_${runID}"
		mkdir -pv ${runDir}
		cd ${runDir}
		echo "#!/bin/bash" > ${runFile}
		echo "source /home/belle2/anatochi/eic/epic/install/setup.sh" >> ${runFile}
		# echo "source /opt/detector/setup.sh" >> ${runFile}
	fi
done

cd -

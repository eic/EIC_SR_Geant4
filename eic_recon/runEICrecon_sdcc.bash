#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

# loop over runs
for runID in {1..10}
do
	echo "[INFO] RunID = ${runID}"

	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	# input directory
	inputDirName="/gpfs02/eic/anatochii/output_eicsim_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
	# output directory
	outputDirName="/gpfs02/eic/anatochii/output_eicrecon_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
	# build directory
	buildDir="${outputDirName}/build_${beamEnergy}GeV_${runID}"
	# number of jobs
	nJobs=1
	# eic-shell
	eicshell='/direct/eic+u/anatochii/eic/eic-shell'
	# eic-setup
	eicsetup='/direct/eic+u/anatochii/eic/epic/install/bin/thisepic.sh'
	#--------------------------------------------------#
	#--------------------------------------------------#

	echo "[INFO] inputDirName = ${inputDirName}"
	echo "[INFO] outputDirName = ${outputDirName}"
	echo "[INFO] buildDir = ${buildDir}"
	echo "[INFO] nJobs = ${nJobs}"

	# output directory
	mkdir -pv ${outputDirName}

	# build directory
	mkdir -pv ${buildDir}
	cp exe.bash  ${buildDir}
	cd ${buildDir}
	chmod 777 exe.bash

	# submit jobs
	instructionFile="instr.sub"

	echo "executable = exe.bash" > ${instructionFile}
	echo "arguments  = \$(ProcId) ${inputDirName} ${outputDirName} ${eicshell} ${eicsetup}" >> ${instructionFile}
	echo "log        = ${outputDirName}/logJob_\$(ProcId).log" >> ${instructionFile}
	echo "error      = ${outputDirName}/logErr_\$(ProcId).err" >> ${instructionFile}
	echo "queue ${nJobs}" >> ${instructionFile}

	condor_submit ${instructionFile}
	cd -
done

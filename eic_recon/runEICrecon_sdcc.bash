#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

echo "[INFO] Beam Energy ${beamEnergy}GeV"

# loop over runs
for runID in {1..100}
do
	echo "[INFO] RunID = ${runID}"

	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	# input directory
	inputDirName="/gpfs02/eic/anatochii/SynradG4_v6.3.1_20240906_18x275GeV_HepMCfiles_IPphotons/output_eicsim_v6.3.1_20240906_${beamEnergy}GeV_${runID}"
	# output directory
	outputDirName="/gpfs02/eic/anatochii/SynradG4_v6.3.1_20240906_18x275GeV_HepMCfiles_IPphotons/output_eicrecon_v6.3.1_20240906_${beamEnergy}GeV_${runID}"
	# build directory
	buildDir="${outputDirName}/build"
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

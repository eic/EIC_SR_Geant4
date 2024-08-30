#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

for mask in 'nosrmask' 'srmask'
do
	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	inputDirName="/gpfs02/eic/anatochii/output_g4ana_v6.3.1_${mask}_${beamEnergy}GeV_highStat" # input directory
	outputDirName="/gpfs02/eic/anatochii/output_eicsim_v6.3.1_${mask}_${beamEnergy}GeV_highStat" # output directory
	buildDir="${outputDirName}/build_${beamEnergy}GeV_${mask}" # build directory
	nJobs=10000 # number of jobs
	eicshell='/direct/eic+u/anatochii/eic/eic-shell'
	#eicsetup='/direct/eic+u/anatochii/eic/epic/install/setup.sh'
	eicsetup='/direct/eic+u/anatochii/eic/epic/install/bin/thisepic.sh'
	compactFile='/direct/eic+u/anatochii/eic/epic/install/share/epic/epic.xml'
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
	cp exe.bash ${buildDir}
	cd ${buildDir}
	chmod 777 exe.bash

	# submit jobs
	instructionFile="instr.sub"

	echo "executable = exe.bash" > ${instructionFile}
	echo "arguments  = \$(ProcId) ${inputDirName} ${outputDirName} ${eicshell} ${eicsetup} ${compactFile}" >> ${instructionFile}
#	echo "log        = ${outputDirName}/logJob_\$(ProcId).log" >> ${instructionFile}
#	echo "error      = ${outputDirName}/logErr_\$(ProcId).err" >> ${instructionFile}
	echo "queue ${nJobs}" >> ${instructionFile}

	condor_submit ${instructionFile}
	cd -
done

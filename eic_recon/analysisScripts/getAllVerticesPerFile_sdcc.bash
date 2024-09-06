#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

echo "[INFO] Beam Energy ${beamEnergy}GeV"

# loop over run
for runID in {1..10}
do
	echo "[INFO] RunID = ${runID}"

	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	# input directory
	inputDirName="/gpfs02/eic/anatochii/output_eicrecon_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
	# build directory
	buildDir="${inputDirName}/build_anaVtx"
	# pics directory
	picsDir="${inputDirName}/pics_ana"
	# number of jobs
	nJobs=1
	# eic-shell
	eicshell='/direct/eic+u/anatochii/eic/eic-shell'
	# eic-setup
	eicsetup='/direct/eic+u/anatochii/eic/epic/install/bin/thisepic.sh'
	# script
	scriptName='getAllVerticesPerFile.C'
	#--------------------------------------------------#
	#--------------------------------------------------#

	echo "[INFO] inputDirName = ${inputDirName}"
	echo "[INFO] buildDir = ${buildDir}"
	echo "[INFO] picsDir = ${picsDir}"
	echo "[INFO] nJobs = ${nJobs}"
	echo "[INFO] scriptName = ${scriptName}"

	# build directory
	mkdir -pv ${buildDir}
	mkdir -pv ${picsDir}
	cp ${scriptName} ${buildDir}
	cp exeVertices.bash ${buildDir}
	cd ${buildDir}
	chmod 777 exeVertices.bash

	# submit jobs
	instructionFile="instr.sub"

	echo "executable = exeVertices.bash" > ${instructionFile}
	echo "arguments  = \$(ProcId) ${inputDirName} ${inputDirName} ${eicshell} ${eicsetup} ${scriptName}" >> ${instructionFile}
	echo "queue ${nJobs}" >> ${instructionFile}

	condor_submit ${instructionFile}

	echo ''
	echo "[INFO] When HTCondor jobs finished, run in eic-shell: root -l -b -q drawAllVertices.C\\(\\\"${inputDirName}\\\",\\\".vertex\\\",\\\".root\\\",\\\"${picsDir}\\\"\\)"
	cd -
done

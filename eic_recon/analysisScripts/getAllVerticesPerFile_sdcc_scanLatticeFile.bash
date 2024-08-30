#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

for mask in 'nosrmask' 'srmask'
do
	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	inputDirName="/gpfs02/eic/anatochii/output_eicrecon_v6.3.1_${mask}_${beamEnergy}GeV_highStat" # input directory
	buildDir="${inputDirName}/build_${beamEnergy}GeV_${mask}_anaVtx" # build directory
	picsDir="./pics_${beamEnergy}GeV_${mask}_ana" # pics directory
	nJobs=10000 # number of jobs
	eicshell='/direct/eic+u/anatochii/eic/eic-shell'
	eicsetup='/direct/eic+u/anatochii/eic/epic/install/bin/thisepic.sh'
	scriptName='getAllVerticesPerFile.C'
	#--------------------------------------------------#
	#--------------------------------------------------#

	echo "[INFO] inputDirName = ${inputDirName}"
	echo "[INFO] buildDir = ${buildDir}"
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
#	echo "log        = ${inputDirName}/logAllVerticesJob_\$(ProcId).log" >> ${instructionFile}
#	echo "error      = ${inputDirName}/logAllVerticesErr_\$(ProcId).err" >> ${instructionFile}
	echo "queue ${nJobs}" >> ${instructionFile}

	condor_submit ${instructionFile}

	echo ''
	echo "[INFO] When HTCondor jobs finished, run in eic-shell: root -l -b -q drawAllVertices.C\\(\\\"${inputDirName}\\\",\\\".vertex\\\",\\\".root\\\",\\\"${picsDir}\\\"\\)"
	cd -
done

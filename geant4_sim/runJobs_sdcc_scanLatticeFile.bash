#!/bin/bash
LANG=en_US
LC_NUMERIC=en_US.UTF-8

date;

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

for mask in 'nosrmask' 'srmask'
do
	echo "[INFO] Beam Energy ${beamEnergy}GeV"
	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	xmlFile="/direct/eic+u/anatochii/EIC_SR_Geant4/geant4_sim/geometry/simParameters_v6.3.1_${beamEnergy}GeV_${mask}.xml" # simulation geometry settings
	nParticles=10000 # number of primary particles per job
	outputDir="/gpfs02/eic/anatochii/output_v6.3.1_${mask}_${beamEnergy}GeV_highStat" # output directory
	buildDir="./build_${beamEnergy}GeV_${mask}" # build directory
	nJobs=10000 # number of jobs
	eicshell='/direct/eic+u/anatochii/eic/eic-shell'
	#--------------------------------------------------#
	#--------------------------------------------------#

	echo ""
	echo "[INFO] Input parameters:"
	echo " xmlFile = ${xmlFile}"
	echo " nParticles = ${nParticles}"
	echo " nJobs = ${nJobs}"
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

	compileFile="compile.sh"

	# config
	echo "make clean" > ${compileFile}
	echo "cmake ../" >> ${compileFile}
	echo "make -j4" >> ${compileFile}

	chmod 777 ${compileFile}

	. ${eicshell} -- ./${compileFile}

	chmod 777 runAll_sdcc.bash

	mkdir -pv ./macFiles

	# submit jobs
	instructionFile="instr.sub"

	echo "executable = runAll_sdcc.bash" > ${instructionFile}
	echo "arguments  = \$(ProcId) $nParticles $xmlFile $eicshell $outputDir" >> ${instructionFile}
	echo "log        = ${outputDir}/logJob_\$(ProcId).log" >> ${instructionFile}
	echo "error      = ${outputDir}/logErr_\$(ProcId).err" >> ${instructionFile}
	echo "queue ${nJobs}" >> ${instructionFile}

	condor_submit ${instructionFile}
	cd -
done

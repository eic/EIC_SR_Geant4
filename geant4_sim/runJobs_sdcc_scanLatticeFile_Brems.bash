#!/bin/bash
LANG=en_US
LC_NUMERIC=en_US.UTF-8

date;

beamEnergy=$1 # '18x275', '10x275', or '5x100'

echo "[INFO] Beam Energy ${beamEnergy}GeV"

for farBWDmask in 'farBWDmask' 'nofarBWDmask'
do
	echo "[INFO] Far-BWD SR mask = ${farBWDmask}"

	# get the current/work dir
	workdir=${PWD}

	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	xmlFile="../geometry/simParameters_v6.3.1_${beamEnergy}GeV_Brems_${farBWDmask}.xml" # simulation geometry settings
	nParticles=1000000 # number of primary particles per job
	outputDir="/gpfs02/eic/anatochii/output_v6.3.1_${beamEnergy}GeV_20241014_Brems_${farBWDmask}" # output directory
	buildDir="${outputDir}/build_${beamEnergy}GeV_Brems_${farBWDmask}" # build directory
	nJobs=1 # number of jobs
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

	# copy the geometry directory
	cp -r ${workdir}/geometry ../

	# copy the materials directory
	cp -r ${workdir}/materials ../

	compileFile="compile.sh"

	# config
	echo "make clean" > ${compileFile}
	echo "cmake ${workdir}" >> ${compileFile}
	echo "make -j4" >> ${compileFile}

	chmod 777 ${compileFile}

	. ${eicshell} -- ./${compileFile}

	chmod 777 runAll_sdcc.bash

	mkdir -pv ./macFiles

	# submit jobs
	instructionFile="instr.sub"

	echo "executable = runAll_sdcc.bash" > ${instructionFile}
	echo "arguments  = \$(ProcId) $nParticles $xmlFile $eicshell $outputDir" >> ${instructionFile}
	#	echo "log        = ${outputDir}/logJob_\$(ProcId).log" >> ${instructionFile}
	#	echo "error      = ${outputDir}/logErr_\$(ProcId).err" >> ${instructionFile}
	echo "queue ${nJobs}" >> ${instructionFile}

	condor_submit ${instructionFile}
	cd -
done

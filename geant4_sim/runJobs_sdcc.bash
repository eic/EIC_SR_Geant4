#!/bin/bash
LANG=en_US
LC_NUMERIC=en_US.UTF-8

date;

beamEnergy=$1 # '18x275' or '10x275' or '5x100'
runID=$2

echo "[INFO] Beam Energy ${beamEnergy}GeV"
echo "[INFO] RunID = ${runID}"

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
# simulation geometry settings
xmlFile="/direct/eic+u/anatochii/EIC_SR_Geant4/geant4_sim/geometry/simParameters_v6.3.1_${beamEnergy}GeV.xml"
# number of primary particles per job
nParticles=10000
# output directory
outputDir="/gpfs02/eic/anatochii/output_g4sim_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
# build directory
buildDir="./build_${beamEnergy}GeV_${runID}"
# number of jobs
nJobs=10000 
# eic-shell
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
#echo "log        = ${outputDir}/logJob_\$(ProcId).log" >> ${instructionFile}
#echo "error      = ${outputDir}/logErr_\$(ProcId).err" >> ${instructionFile}
echo "queue ${nJobs}" >> ${instructionFile}

condor_submit ${instructionFile}
cd -

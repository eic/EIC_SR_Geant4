#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
inputDirName="/gpfs02/eic/anatochii/${1}_highstat_output_eicrecon_v6.2_srmask5_1" # output directory
buildDir="./build_${1}" # build directory
nJobs=20000 # number of jobs
eicshell='/direct/eic+u/anatochii/eic/eic-shell'
eicsetup='/direct/eic+u/anatochii/eic/epic/install/setup.sh'
scriptName='getAllHitsPerFile.C'
#--------------------------------------------------#
#--------------------------------------------------#

echo "[INFO] inputDirName = ${inputDirName}"
echo "[INFO] buildDir = ${buildDir}"
echo "[INFO] nJobs = ${nJobs}"
echo "[INFO] scriptName = ${scriptName}"

# build directory
mkdir -pv ${buildDir}
cd ${buildDir}
cp ../${scriptName} ./

cp ../exe.bash  ./
chmod 777 exe.bash

# submit jobs
instructionFile="instr.sub"

echo "executable = exe.bash" > ${instructionFile}
echo "arguments  = \$(ProcId) ${inputDirName} ${inputDirName} ${eicshell} ${eicsetup} ${scriptName}" >> ${instructionFile}
echo "log        = ${inputDirName}/logAllHitsJob_\$(ProcId).log" >> ${instructionFile}
echo "error      = ${inputDirName}/logAllHitsErr_\$(ProcId).err" >> ${instructionFile}
echo "queue ${nJobs}" >> ${instructionFile}

condor_submit ${instructionFile}

echo ''
echo "[INFO] When HTCondor jobs finished, run in eic-shell: root -l -b -q drawAllHits.C\\(\\\"${inputDirName}\\\",\\\".hist\\\",\\\".root\\\",\\\"./pics\\\"\\)"

#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
inputDirName="/gpfs02/eic/anatochii/${1}_highstat_output_eicrecon_v6.2_srmask5_1_ipcut4" # output directory
buildDir="./build_${1}" # build directory
picsDir="./pics_${1}" # pics directory
nJobs=10000 # number of jobs
eicshell='/direct/eic+u/anatochii/eic/eic-shell'
eicsetup='/direct/eic+u/anatochii/eic/epic/install/setup.sh'
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
cd ${buildDir}
cp ../${scriptName} ./

cp ../exeVertices.bash  ./
chmod 777 exeVertices.bash

# submit jobs
instructionFile="instr.sub"

echo "executable = exeVertices.bash" > ${instructionFile}
echo "arguments  = \$(ProcId) ${inputDirName} ${inputDirName} ${eicshell} ${eicsetup} ${scriptName}" >> ${instructionFile}
echo "log        = ${inputDirName}/logAllVerticesJob_\$(ProcId).log" >> ${instructionFile}
echo "error      = ${inputDirName}/logAllVerticesErr_\$(ProcId).err" >> ${instructionFile}
echo "queue ${nJobs}" >> ${instructionFile}

condor_submit ${instructionFile}

echo ''
echo "[INFO] When HTCondor jobs finished, run in eic-shell: root -l -b -q drawAllVertices.C\\(\\\"${inputDirName}\\\",\\\".vertex\\\",\\\".root\\\",\\\"${picsDir}\\\"\\)"

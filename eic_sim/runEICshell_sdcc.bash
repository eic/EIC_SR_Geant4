#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
inputDirName='/gpfs02/eic/anatochii/highstat_output_g4ana_v6.2_srmask5_1' # input directory
outputDirName='/gpfs02/eic/anatochii/highstat_output_eicsim_v6.2_srmask5_1' # output directory
buildDir='./build' # build directory
nJobs=10000 # number of jobs
eicshell='/direct/eic+u/anatochii/eic/eic-shell'
eicsetup='/direct/eic+u/anatochii/eic/epic/install/setup.sh'
compactFile='/direct/eic+u/anatochii/eic/epic/install/share/epic/epic.xml'
#--------------------------------------------------#
#--------------------------------------------------#

echo "[INFO] inputDirName = ${inputDirName}"
echo "[INFO] outputDirName = ${outputDirName}"
echo "[INFO] buildDir = ${buildDir}"
echo "[INFO] nJobs = ${nJobs}"
echo "[INFO] queue = ${queue}"

# output directory
mkdir -pv ${outputDirName}
# build directory
rm -rf ${buildDir}
mkdir -pv ${buildDir}
cd ${buildDir}
cp ../exe.bash  ./
chmod 777 exe.bash

# submit jobs
instructionFile="instr.sub"

echo "executable = exe.bash" > ${instructionFile}
echo "arguments  = \$(ProcId) ${inputDirName} ${outputDirName} ${eicshell} ${eicsetup} ${compactFile}" >> ${instructionFile}
echo "log        = ${outputDirName}/logJob_\$(ProcId).log" >> ${instructionFile}
echo "error      = ${outputDirName}/logErr_\$(ProcId).err" >> ${instructionFile}
echo "queue ${nJobs}" >> ${instructionFile}

condor_submit ${instructionFile}

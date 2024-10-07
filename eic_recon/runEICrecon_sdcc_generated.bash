#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
# input directory
inputDirName="/gpfs02/eic/anatochii/Generated_HepMC_Files_SR_on_IP/output_eicsim"
# output directory
outputDirName="/gpfs02/eic/anatochii/Generated_HepMC_Files_SR_on_IP/output_eicrecon" 
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

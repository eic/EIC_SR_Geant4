#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
inputDirName="/gpfs02/eic/anatochii/${1}_highstat_output_eicsim_v6.2_srmask5_1_ipcut4" # input directory
outputDirName="/gpfs02/eic/anatochii/${1}_highstat_output_eicrecon_v6.2_srmask5_1_ipcut4" # output directory
buildDir="./build_${1}" # build directory
nJobs=10000 # number of jobs
eicshell='/direct/eic+u/anatochii/eic/eic-shell'
#eicsetup='/direct/eic+u/anatochii/eic/epic/install/setup.sh'
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
cd ${buildDir}
cp ../exe.bash  ./
chmod 777 exe.bash

# submit jobs
instructionFile="instr.sub"

echo "executable = exe.bash" > ${instructionFile}
echo "arguments  = \$(ProcId) ${inputDirName} ${outputDirName} ${eicshell} ${eicsetup}" >> ${instructionFile}
echo "log        = ${outputDirName}/logJob_\$(ProcId).log" >> ${instructionFile}
echo "error      = ${outputDirName}/logErr_\$(ProcId).err" >> ${instructionFile}
echo "queue ${nJobs}" >> ${instructionFile}

condor_submit ${instructionFile}

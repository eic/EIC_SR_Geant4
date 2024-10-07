#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
# input directory
inputDirName="/gpfs02/eic/anatochii/Generated_HepMC_Files_SR_on_IP/output_eicrecon"
# build directory
buildDir="${inputDirName}/build_anaHits"
# pics directory
picsDir="${inputDirName}/pics_ana"
# number of jobs
nJobs=1
# eic-shell
eicshell='/direct/eic+u/anatochii/eic/eic-shell'
# eic-setup
eicsetup='/direct/eic+u/anatochii/eic/epic/install/bin/thisepic.sh'
# script
scriptName='getAllHitsPerFile.C'
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
cp exeHits.bash ${buildDir}
cd ${buildDir}
chmod 777 exeHits.bash

# submit jobs
instructionFile="instr.sub"

echo "executable = exeHits.bash" > ${instructionFile}
echo "arguments  = \$(ProcId) ${inputDirName} ${inputDirName} ${eicshell} ${eicsetup} ${scriptName}" >> ${instructionFile}
echo "queue ${nJobs}" >> ${instructionFile}

condor_submit ${instructionFile}

cd -

#cd ~/; source goToConda.sh; cd -; time root -l -b -q drawAllHits.C\(\"/gpfs02/eic/anatochii/Generated_HepMC_Files_SR_on_IP/output_eicrecon\",\".hist\",\".root\",\"/gpfs02/eic/anatochii/Generated_HepMC_Files_SR_on_IP/output_eicrecon/pics_ana\"\);

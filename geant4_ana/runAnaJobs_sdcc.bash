#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

runID=$2

echo "[INFO] Beam Energy ${beamEnergy}GeV"
echo "[INFO] RunID = ${runID}"

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
# input directory
mainInDir="/gpfs02/eic/anatochii/output_g4sim_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
# input tree name
treeName='tree'
# output directory
outputDir="/gpfs02/eic/anatochii/output_g4ana_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
# build directory
buildDir="./build_${beamEnergy}GeV_${runID}"
nJobs=10000 # number of jobs
# in the custom-built Geant4 coordinate system
minZ=-79.9 # [cm] minimum Z
maxZ=66.9 # [cm] maximum Z
# eic-shell
eicshell='/direct/eic+u/anatochii/eic/eic-shell'
# target radius to backtrack absorbed SR photons [mm]
r_target=-1
#--------------------------------------------------#
#--------------------------------------------------#

echo ""
echo "[INFO] mainInDir = ${mainInDir}"
echo "[INFO] treeName = ${treeName}"
echo "[INFO] outputDir = ${outputDir}"
echo "[INFO] buildDir = ${buildDir}"
echo "[INFO] nJobs = ${nJobs}"
echo "[INFO] minZ = ${minZ} [cm]"
echo "[INFO] maxZ = ${maxZ} [cm]"
echo "[INFO] r_target = ${r_target} [mm]"
echo ""

# output directory
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

# submit jobs
instructionFile="instr.sub"

echo "#!/bin/bash" > run.bash
echo "echo \"./exe ${mainInDir} output_\${1}.root ${outputDir} output_ana_\${1} ${treeName} ${minZ} ${maxZ} ${r_target} &> ${outputDir}/logExe_\${1}.txt\" > exe_\${1}.bash" >> run.bash
echo "chmod 777 exe_\${1}.bash" >> run.bash
echo ". ${eicshell} -- ./exe_\${1}.bash" >> run.bash
chmod 777 run.bash

echo "executable = run.bash" > ${instructionFile}
echo "arguments  = \$(ProcId)" >> ${instructionFile}
#	echo "log        = ${outputDir}/logJob_\$(ProcId).log" >> ${instructionFile}
#	echo "error      = ${outputDir}/logErr_\$(ProcId).err" >> ${instructionFile}
echo "queue ${nJobs}" >> ${instructionFile}

condor_submit ${instructionFile}
cd -

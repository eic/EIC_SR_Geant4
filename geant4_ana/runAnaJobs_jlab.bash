#!/bin/bash

LANG=en_US
LC_NUMERIC=en_US.UTF-8

date;

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

echo "[INFO] Beam Energy ${beamEnergy}GeV"
#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
# input directory
mainInDir="/work/eic/users/anatochii/output_v6.3.1_20240904_${beamEnergy}GeV"
# input tree name
treeName='tree'
# output directory
outputDir="/work/eic/users/anatochii/output_g4ana_v6.3.1_20240904_${beamEnergy}GeV"
# build directory
buildDir="./build_${beamEnergy}GeV"
# number of jobs
nJobs=1000
# in the custom-built Geant4 coordinate system
minZ=-79.9 # [cm] minimum Z
maxZ=66.9 # [cm] maximum Z
# eic-shell
eicshell='/home/natochii/eic/eic-shell'
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
instructionFile="instrJob.sh"

echo "#!/bin/bash" > run.bash
echo "echo \"./exe ${mainInDir} output_\${1}.root ${outputDir} output_ana_\${1} ${treeName} ${minZ} ${maxZ} ${r_target}\" > exe_\${1}.bash" >> run.bash
echo "chmod 777 exe_\${1}.bash" >> run.bash
echo ". ${eicshell} -- ./exe_\${1}.bash" >> run.bash
chmod 777 run.bash

echo "#!/bin/bash" > ${instructionFile}
echo "#" >> ${instructionFile}
echo "#SBATCH --job-name=G4-ana" >> ${instructionFile}
echo "#SBATCH --nodes=1" >> ${instructionFile}
echo "#SBATCH --ntasks=1" >> ${instructionFile}
echo "#SBATCH --cpus-per-task=1" >> ${instructionFile}
#echo "#SBATCH --mem=1G" >> ${instructionFile} # total memory per node
echo "#SBATCH --mem-per-cpu=8G" >> ${instructionFile} # memory per cpu-core
echo "#SBATCH --partition=production" >> ${instructionFile}
echo "#SBATCH --time=01:00:00" >> ${instructionFile}
echo "#SBATCH -o /dev/null" >> ${instructionFile}
echo "#SBATCH --array=1-${nJobs}" >> ${instructionFile}
echo "" >> ${instructionFile} 
echo "./run.bash \${SLURM_ARRAY_TASK_ID} &> ${outputDir}/logExe_\${SLURM_ARRAY_TASK_ID}.txt" >> ${instructionFile}

sbatch ${instructionFile}
cd -

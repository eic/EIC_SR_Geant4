#!/bin/bash

date 

# global path for output files
globalPath=$1
echo "[INFO] Global PATH: ${globalPath}"

# eic-shell: '/home/username/eic/eic-shell'
eicshell=$2
echo "[INFO] Eic-shell: ${eicshell}"

beamEnergy='18x275'
echo "[INFO] Beam Energy ${beamEnergy}GeV"

# build directory
mkdir -pv build
cd build

# compile the code
compileFile="compile.sh"
echo "make clean" > ${compileFile}
echo "cmake ../" >> ${compileFile}
echo "make -j1" >> ${compileFile}
chmod 777 ${compileFile}
. ${eicshell} -- ./${compileFile}

# loop over runs
for runID in {1..100}
do
	echo ""
	echo "################################ RUNID = ${runID} ################################";

	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	# output directory
	inputDir="${globalPath}/output_g4ana_v6.3.1_${beamEnergy}GeV_${runID}"
	nJobs=10000 # number of jobs
	#--------------------------------------------------#
	#--------------------------------------------------#

	echo ""
	echo "[INFO] inputDir = ${inputDir}"
	echo "[INFO] nJobs = ${nJobs}"
	echo ""

	# submit jobs
	instructionFile="instr_${runID}.sub"
	runFile="run_${runID}.bash"

	echo "#!/bin/bash" > ${runFile}
	echo "./exe ${inputDir}.hepmc ${inputDir}/output_ana_ ${nJobs} &> ./log_${runID}.txt" >> ${runFile}
	chmod 777 ${runFile}
	
	echo "#!/bin/bash" > ${instructionFile}
	echo "#" >> ${instructionFile}
	echo "#SBATCH --job-name=Merge-hepmc" >> ${instructionFile}
	echo "#SBATCH --nodes=1" >> ${instructionFile}
	echo "#SBATCH --ntasks=1" >> ${instructionFile}
	echo "#SBATCH --cpus-per-task=1" >> ${instructionFile}
	#echo "#SBATCH --mem=1G" >> ${instructionFile} # total memory per node
	echo "#SBATCH --mem-per-cpu=8G" >> ${instructionFile} # memory per cpu-core
	echo "#SBATCH --partition=production" >> ${instructionFile}
	echo "#SBATCH --time=01:00:00" >> ${instructionFile}
	echo "#SBATCH -o /dev/null" >> ${instructionFile}
	echo "" >> ${instructionFile} 
	echo ". ${eicshell} -- ./${runFile}" >> ${instructionFile}
	
	sbatch ${instructionFile}
done
cd -

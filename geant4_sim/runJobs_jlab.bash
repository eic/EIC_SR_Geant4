#!/bin/bash

LANG=en_US
LC_NUMERIC=en_US.UTF-8

date;

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

echo "[INFO] Beam Energy ${beamEnergy}GeV"

for runID in {1..10}
do
	echo ""
	echo "################################ RUNID = ${runID} ################################";

	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	# simulation geometry settings
	xmlFile="/home/natochii/EIC_SR_Geant4/geant4_sim/geometry/simParameters_v6.3.1_${beamEnergy}GeV.xml"
	# number of primary particles per job
	nParticles=10000
	# output directory
	outputDir="/work/eic/users/anatochii/output_g4sim_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
	buildDir="./build_${beamEnergy}GeV_${runID}" # build directory
	# number of jobs
	nJobs=10000 # SLURM: MaxArraySize=10001 
	# eic-shell
	eicshell='/home/natochii/eic/eic-shell'
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

	chmod 777 runAll_jlab.bash

	# submit jobs
	instructionFile="instrJob.sh"

	echo "#!/bin/bash" > ${instructionFile}
	echo "#" >> ${instructionFile}
	echo "#SBATCH --job-name=G4-sim" >> ${instructionFile}
	echo "#SBATCH --nodes=1" >> ${instructionFile}
	echo "#SBATCH --ntasks=1" >> ${instructionFile}
	echo "#SBATCH --cpus-per-task=1" >> ${instructionFile}
	#echo "#SBATCH --mem=1G" >> ${instructionFile} # total memory per node
	echo "#SBATCH --mem-per-cpu=1G" >> ${instructionFile} # memory per cpu-core
	echo "#SBATCH --partition=production" >> ${instructionFile}
	echo "#SBATCH --time=03:00:00" >> ${instructionFile}
	echo "#SBATCH -o /dev/null" >> ${instructionFile}
	echo "#SBATCH --array=1-${nJobs}" >> ${instructionFile}
	echo "" >> ${instructionFile} 
	echo "./runAll_jlab.bash \${SLURM_ARRAY_TASK_ID} ${nParticles} ${xmlFile} ${eicshell} ${outputDir} &> ${outputDir}/logExe_\${SLURM_ARRAY_TASK_ID}.txt" >> ${instructionFile}

	sbatch ${instructionFile}
	cd -
done

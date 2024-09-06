#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

# loop over runs
for runID in {1..10}
do
	echo "[INFO] RunID = ${runID}"

	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	# input directory
	inputDirName="/work/eic/users/anatochii/output_eicsim_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
	# output directory
	outputDirName="/work/eic/users/anatochii/output_eicrecon_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
	# build directory
	buildDir="${outputDirName}/build_${beamEnergy}GeV_${runID}"
	# number of jobs
	nJobs=1
	# eic-shell
	eicshell='/home/natochii/eic/eic-shell'
	# eic-setup
	eicsetup='/home/natochii/eic/epic/install/bin/thisepic.sh'
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

	echo "#!/bin/bash" > ${instructionFile}
	echo "#" >> ${instructionFile}
	echo "#SBATCH --job-name=Merge-hepmc" >> ${instructionFile}
	echo "#SBATCH --nodes=1" >> ${instructionFile}
	echo "#SBATCH --ntasks=1" >> ${instructionFile}
	echo "#SBATCH --cpus-per-task=1" >> ${instructionFile}
	#echo "#SBATCH --mem=1G" >> ${instructionFile} # total memory per node
	echo "#SBATCH --mem-per-cpu=8G" >> ${instructionFile} # memory per cpu-core
	echo "#SBATCH --partition=production" >> ${instructionFile}
	echo "#SBATCH --time=03:00:00" >> ${instructionFile}
	echo "#SBATCH -o /dev/null" >> ${instructionFile}
	echo "#SBATCH --array=1-${nJobs}" >> ${instructionFile}
	echo "" >> ${instructionFile} 
	echo "./exe.bash \${SLURM_ARRAY_TASK_ID} ${inputDirName} ${outputDirName} ${eicshell} ${eicsetup}" >> ${instructionFile}
	
	sbatch ${instructionFile}
	cd -
done

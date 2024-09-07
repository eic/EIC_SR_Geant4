#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

echo "[INFO] Beam Energy ${beamEnergy}GeV"

# loop over run
for runID in {1..10}
do
	echo "[INFO] RunID = ${runID}"

	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	# input directory
	inputDirName="/work/eic/users/anatochii/output_eicrecon_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
	# build directory
	buildDir="${inputDirName}/build_anaHits"
	# pics directory
	picsDir="${inputDirName}/pics_ana"
	# number of jobs
	nJobs=1
	# eic-shell
	eicshell='/home/natochii/eic/eic-shell'
	# eic-setup
	eicsetup='/home/natochii/eic/epic/install/bin/thisepic.sh'
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
	echo "./exeHits.bash \${SLURM_ARRAY_TASK_ID} ${inputDirName} ${inputDirName} ${eicshell} ${eicsetup} ${scriptName}" >> ${instructionFile}
	
	sbatch ${instructionFile}

	cd -
done

#cd ~/; source goToConda.sh; cd -; beamEnergy='18x275'; for runID in {1..10};do echo "RunID = ${runID}"; time root -l -b -q drawAllHits.C\(\"/work/eic/users/anatochii/output_eicrecon_v6.3.1_20240904_${beamEnergy}GeV_${runID}\",\".hist\",\".root\",\"/work/eic/users/anatochii/output_eicrecon_v6.3.1_20240904_${beamEnergy}GeV_${runID}/pics_ana\"\); echo ''; done

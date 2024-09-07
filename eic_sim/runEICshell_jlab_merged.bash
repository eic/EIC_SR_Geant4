#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

echo "[INFO] Beam Energy ${beamEnergy}GeV"

# loop over runs
for runID in {1..10}
do
	echo "[INFO] RunID = ${runID}"

	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	# input filename
	inputFileName="/work/eic/users/anatochii/output_g4ana_v6.3.1_20240904_${beamEnergy}GeV_${runID}.hepmc"
	# output directory
	outputDirName="/work/eic/users/anatochii/output_eicsim_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
	# build directory
	buildDir="${outputDirName}/build"
	# number of jobs
	nJobs=1
	# eic-shell
	eicshell='/home/natochii/eic/eic-shell'
	# eic-setup
	eicsetup='/home/natochii/eic/epic/install/bin/thisepic.sh'
	# eic compact file
	compactFile='/home/natochii/eic/epic/install/share/epic/epic.xml'
	#--------------------------------------------------#
	#--------------------------------------------------#

	echo "[INFO] inputFileName = ${inputFileName}"
	echo "[INFO] outputDirName = ${outputDirName}"
	echo "[INFO] buildDir = ${buildDir}"
	echo "[INFO] nJobs = ${nJobs}"

	# output directory
	mkdir -pv ${outputDirName}

	# build directory
	mkdir -pv ${buildDir}
	cd ${buildDir}

	# create an exe file
	exeFile='exe.bash'
	echo "#!/bin/bash" > ${exeFile}
	echo "jobID=\$1" >> ${exeFile}
	echo "inputFileName=\$2" >> ${exeFile}
	echo "outputDirName=\$3" >> ${exeFile}
	echo "eicshell=\$4" >> ${exeFile}
	echo "eicsetup=\$5" >> ${exeFile}
	echo "compactFile=\$6" >> ${exeFile}

	echo "runFile=\"runEICsim.sh\"" >> ${exeFile}
	echo "echo \"source \${eicsetup}\" >> \${runFile}" >> ${exeFile}
	echo "echo -ne \"time npsim --runType batch --numberOfEvents -1 --compactFile \${compactFile}\" >> \${runFile}" >> ${exeFile}
	echo "echo \" --inputFiles \${inputFileName} --outputFile \${outputDirName}/output_eicsim_\${jobID}.edm4hep.root &> \${outputDirName}/logExe_\${jobID}.txt\" >> \${runFile}" >> ${exeFile}
	echo "chmod 777 \${runFile}" >> ${exeFile}
	echo ". \${eicshell} -- ./\${runFile}" >> ${exeFile}

	chmod 777 ${exeFile}

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
	echo "./${exeFile} \${SLURM_ARRAY_TASK_ID} ${inputFileName} ${outputDirName} ${eicshell} ${eicsetup} ${compactFile}" >> ${instructionFile}
	
	sbatch ${instructionFile}
	cd -
done

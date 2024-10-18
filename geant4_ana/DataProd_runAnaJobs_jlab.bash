#!/bin/bash

LANG=en_US
LC_NUMERIC=en_US.UTF-8

date;

# global path for output files
globalPath=$1
echo "[INFO] Global PATH: ${globalPath}"

# eic-shell: '/home/username/eic/eic-shell'
eicshell=$2
echo "[INFO] Eic-shell: ${eicshell}"

beamEnergy='18x275'
echo "[INFO] Beam Energy ${beamEnergy}GeV"

for runID in {1..100}
do
	echo ""
	echo "################################ RUNID = ${runID} ################################";

	# get the current/work dir
	workdir=${PWD}

	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	# input directory
	mainInDir="${globalPath}/output_g4sim_v6.3.1_${beamEnergy}GeV_${runID}"
	# input tree name
	treeName='tree'
	# output directory
	outputDir="${globalPath}/output_g4ana_v6.3.1_${beamEnergy}GeV_${runID}"
	# build directory
	buildDir="${outputDir}/build"
	# number of jobs
	nJobs=10000
	# in the custom-built Geant4 coordinate system
	minZ=-79.9 # [cm] minimum Z
	maxZ=66.9 # [cm] maximum Z
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
	echo "cmake ${workdir}/" >> ${compileFile}
	echo "make -j4" >> ${compileFile}

	chmod 777 ${compileFile}

	. ${eicshell} -- ./${compileFile}

	# create a multi-line script
	echo "#!/bin/bash" > run.bash
	echo "minFileID=\${1}" >> run.bash
	echo "maxFileID=\${2}" >> run.bash
	echo "jobID=\${3}" >> run.bash
	echo "echo \"date\" > exe_\${jobID}.bash" >> run.bash
	echo "for (( i=\${minFileID}; i<=\${maxFileID}; i++ ))" >> run.bash
	echo "do" >> run.bash
	echo "	echo \"./exe ${mainInDir} output_\${i}.root ${outputDir} output_ana_\${i} ${treeName} ${minZ} ${maxZ} ${r_target}\" >> exe_\${jobID}.bash" >> run.bash
	echo "done" >> run.bash
	echo "chmod 777 exe_\${jobID}.bash" >> run.bash
	echo ". ${eicshell} -- ./exe_\${jobID}.bash" >> run.bash
	chmod 777 run.bash

	# submit jobs
	firstID=1
	nJobSubmitted=0
	for (( i=${firstID}; i<=${nJobs}; i++ ))
	do
		if (( i % 20 == 0 || i == ${nJobs} ))
		then
			lastID=$i
			echo "[INFO] Range: ${firstID} - ${lastID}"
			instructionFile="instrJob_${firstID}-${lastID}.sh"
			echo "#!/bin/bash" > ${instructionFile}
			echo "#" >> ${instructionFile}
			echo "#SBATCH --job-name=G4-ana" >> ${instructionFile}
			echo "#SBATCH --nodes=1" >> ${instructionFile}
			echo "#SBATCH --ntasks=1" >> ${instructionFile}
			echo "#SBATCH --cpus-per-task=1" >> ${instructionFile}
			#echo "#SBATCH --mem=1G" >> ${instructionFile} # total memory per node
			echo "#SBATCH --mem-per-cpu=8G" >> ${instructionFile} # memory per cpu-core
			echo "#SBATCH --partition=production" >> ${instructionFile}
			echo "#SBATCH --time=03:00:00" >> ${instructionFile}
			echo "#SBATCH -o /dev/null" >> ${instructionFile}
			echo "" >> ${instructionFile} 
			echo "./run.bash ${firstID} ${lastID} ${nJobSubmitted} &> ${outputDir}/logExe_ana_${nJobSubmitted}.txt" >> ${instructionFile}

			sbatch ${instructionFile}
			nJobSubmitted=$((nJobSubmitted+1))
			firstID=$((i+1))
		fi
	done
	echo "[INFO] nJobSubmitted = ${nJobSubmitted}"
	cd -
done

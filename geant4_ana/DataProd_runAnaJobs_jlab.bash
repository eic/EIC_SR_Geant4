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

dataProdFlag=1
echo "[INFO] Data prod. flag = ${dataProdFlag}"

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
	# select the region from where SR originates
	minVtxZ=-10000 # [cm] minimum Z
	maxVtxZ=10000 # [cm] maximum Z
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
	echo "[INFO] minVtxZ = ${minVtxZ} [cm]"
	echo "[INFO] maaVtxZ = ${maxVtxZ} [cm]"
	echo ""

	# output directory
	mkdir -pv ${outputDir}

	# build directory
	rm -rf ${buildDir}
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
	echo "	echo \"./exe ${mainInDir} output_\${i} ${outputDir} output_ana_\${i} ${minZ} ${maxZ} ${minVtxZ} ${maxVtxZ} ${dataProdFlag}\" >> exe_\${jobID}.bash" >> run.bash
	echo "done" >> run.bash
	echo "chmod 777 exe_\${jobID}.bash" >> run.bash
	echo ". ${eicshell} -- ./exe_\${jobID}.bash" >> run.bash
	chmod 777 run.bash

	# submit jobs
	firstID=1
	nJobSubmitted=0
	for (( i=${firstID}; i<=${nJobs}; i++ ))
	do
		if (( i % 1000 == 0 || i == ${nJobs} ))
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
			echo "#SBATCH --mem-per-cpu=3G" >> ${instructionFile} # memory per cpu-core
			echo "#SBATCH --partition=production" >> ${instructionFile}
			echo "#SBATCH --time=10:00:00" >> ${instructionFile}
#			echo "#SBATCH -o /dev/null" >> ${instructionFile}
			echo "#SBATCH --output=${outputDir}/logJob_%A_%a_%j.out" >> ${instructionFile}
			echo "#SBATCH --error=${outputDir}/logErr_%A_%a_%j.err" >> ${instructionFile}
			echo "" >> ${instructionFile} 
			echo "./run.bash ${firstID} ${lastID} ${nJobSubmitted} &> ${outputDir}/logExe_ana_${nJobSubmitted}.txt" >> ${instructionFile}

			sbatch ${instructionFile}
			nJobSubmitted=$((nJobSubmitted+1))
			firstID=$((i+1))
		fi
	done
	echo "[INFO] nJobSubmitted = ${nJobSubmitted}"
	cd -

	# Wait before submitting the next bunch of jobs
	pd=$(squeue -u $USER -n G4-ana --state=PENDING | grep -c .)
	rn=$(squeue -u $USER -n G4-ana --state=RUNNING | grep -c .)
	while [ "$pd" -gt 10 ] || [ "$rn" -gt 20 ]
	do
		echo -ne "___ waiting 10 min for the job to complete ... "
		date
		sleep 600
		pd=$(squeue -u $USER -n G4-ana --state=PENDING | grep -c .)
		rn=$(squeue -u $USER -n G4-ana --state=RUNNING | grep -c .)
	done
	date
done

#!/bin/bash

LANG=en_US
LC_NUMERIC=en_US.UTF-8

date;

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

echo "[INFO] Beam Energy ${beamEnergy}GeV"

for runID in 1 #{1..2}
do
	echo ""
	echo "################################ RUNID = ${runID} ################################";

	# get the current/work dir
	workdir=${PWD}

	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	# simulation geometry settings
	xmlFile="../geometry/simParameters_v6.3.1_${beamEnergy}GeV.xml"
	# number of primary particles per job
	nParticles=10000
	# output directory
	outputDir="/pscratch/sd/n/natochii/output_g4sim_v6.3.1_20240906_${beamEnergy}GeV_${runID}"
	# build directory
	buildDir="${outputDir}/build"
	# number of jobs
	nJobs=50 # NERSC SLURM: = number of nodes 
	# eic-shell
	eicshell='/global/homes/n/natochii/eic/eic-shell'
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

	# copy the geometry directory
	cp -r ${workdir}/geometry ../

	# copy the materials directory
	cp -r ${workdir}/materials ../

	compileFile="compile.sh"

	# config
	echo "make clean" > ${compileFile}
	echo "cmake ${workdir}/" >> ${compileFile}
	echo "make -j4" >> ${compileFile}

	chmod 777 ${compileFile}

	. ${eicshell} -- ./${compileFile}

	chmod 777 runAll_nersc.bash

	# submit jobs
	instructionFile="instrJob.sh"

	echo "#!/bin/bash" > ${instructionFile}
	echo "#" >> ${instructionFile}
	echo "#SBATCH --job-name=G4-sim" >> ${instructionFile}
	echo "#SBATCH --nodes=1" >> ${instructionFile}
	echo "#SBATCH --ntasks=200" >> ${instructionFile}
	echo "#SBATCH --cpus-per-task=1" >> ${instructionFile}
	#echo "#SBATCH --mem=1G" >> ${instructionFile} # total memory per node
	#echo "#SBATCH --mem-per-cpu=1G" >> ${instructionFile} # memory per cpu-core
	#echo "#SBATCH --partition=production" >> ${instructionFile}
	echo "#SBATCH --qos=regular" >> ${instructionFile}
	echo "#SBATCH --constraint=cpu" >> ${instructionFile}
	echo "#SBATCH --account=m3763" >> ${instructionFile}
	echo "#SBATCH --time=03:00:00" >> ${instructionFile}
	echo "#SBATCH -o /dev/null" >> ${instructionFile}
	echo "#SBATCH --array=1-${nJobs}" >> ${instructionFile}
	echo "" >> ${instructionFile} 
	echo "export OMP_NUM_THREADS=1" >> ${instructionFile}
	echo "" >> ${instructionFile}
	echo "srun ./runAll_nersc.bash ${nParticles} ${xmlFile} ${eicshell} ${outputDir}" >> ${instructionFile}

	sbatch ${instructionFile}
	cd -
done

#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

echo "[INFO] Beam Energy ${beamEnergy}GeV"

# build directory
mkdir -pv build
cd build

# eic-shell
eicshell='/direct/eic+u/anatochii/eic/eic-shell'

# compile the code
compileFile="compile.sh"
echo "make clean" > ${compileFile}
echo "cmake ../" >> ${compileFile}
echo "make -j1" >> ${compileFile}
chmod 777 ${compileFile}
. ${eicshell} -- ./${compileFile}

# loop over runs
for runID in {1..10}
do
	echo "[INFO] RunID = ${runID}"

	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	# output directory
	inputDir="/gpfs02/eic/anatochii/output_g4ana_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
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
	echo "echo \"./exe ${inputDir}.hepmc ${inputDir}/output_ana_ ${nJobs} &> ./log_${runID}.txt\" > exe_${runID}.bash" >> ${runFile}
	echo "chmod 777 exe_${runID}.bash" >> ${runFile}
	echo ". ${eicshell} -- ./exe_${runID}.bash" >> ${runFile}
	chmod 777 ${runFile}

	echo "executable = ${runFile}" > ${instructionFile}
	echo "queue 1" >> ${instructionFile}

	condor_submit ${instructionFile}
done
cd -

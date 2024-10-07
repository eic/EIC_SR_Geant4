#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

echo "[INFO] Beam Energy ${beamEnergy}GeV"

# build directory
mkdir -pv build
cd build

# eic-shell
eicshell='/home/natochii/eic/eic-shell'

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
	inputFileName="/work/eic/users/anatochii/SynradG4_v6.3.1_20240906_18x275GeV_HepMCfiles_IPphotons/output_g4ana_v6.3.1_20240906_${beamEnergy}GeV_${runID}.hepmc";
	#--------------------------------------------------#
	#--------------------------------------------------#

	echo ""
	echo "[INFO] inputFileName = ${inputFileName}"
	echo ""

	# submit jobs
	instructionFile="instr_${runID}.sub"
	runFile="run_${runID}.bash"

	echo "#!/bin/bash" > ${runFile}
	echo "./exe ${inputFileName} &> ./log_${runID}.txt" >> ${runFile}
	chmod 777 ${runFile}

	. ${eicshell} -- ./${runFile}
done
cd -

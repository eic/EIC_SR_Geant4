#!/bin/bash

for beamEnergy in '18x275' '10x275' '5x100'
do
	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	mainInDir="/gpfs02/eic/anatochii/output_v6.3.1_nosrmask_${beamEnergy}GeV_lowStat" # input directory 
	treeName='tree'
	outputDir="/gpfs02/eic/anatochii/output_g4ana_v6.3.1_nosrmask_${beamEnergy}GeV_lowStat" # output directory "
	buildDir="./build_${beamEnergy}GeV" # build directory
	nJobs=1000 # number of jobs
	# in the custom-built Geant4 coordinate system
	minZ=-10000.0 # [cm] minimum Z
	maxZ=10000.0 # [cm] maximum Z
	eicshell='/direct/eic+u/anatochii/eic/eic-shell'
	#--------------------------------------------------#
	#--------------------------------------------------#

	echo "[INFO] mainInDir = ${mainInDir}"
	echo "[INFO] treeName = ${treeName}"
	echo "[INFO] outputDir = ${outputDir}"
	echo "[INFO] buildDir = ${buildDir}"
	echo "[INFO] nJobs = ${nJobs}"
	echo "[INFO] minZ = ${minZ} [cm]"
	echo "[INFO] maxZ = ${maxZ} [cm]"

	# output directory
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

	# submit jobs
	instructionFile="instr.sub"

	echo "#!/bin/bash" > run.bash
	echo "echo \"./exe \$1 \$2 \$3 output_ana_\$4 \$5 \$6 \$7 &> \$3/logExe_\${4}.txt\" > exe_\${4}.bash" >> run.bash
	echo "chmod 777 exe_\${4}.bash" >> run.bash
	echo ". ${eicshell} -- ./exe_\${4}.bash" >> run.bash
	chmod 777 run.bash

	echo "executable = run.bash" > ${instructionFile}
	echo "arguments  = ${mainInDir} output_\$(ProcId).root ${outputDir} \$(ProcId) ${treeName} ${minZ} ${maxZ}" >> ${instructionFile}
	echo "log        = ${outputDir}/logJob_\$(ProcId).log" >> ${instructionFile}
	echo "error      = ${outputDir}/logErr_\$(ProcId).err" >> ${instructionFile}
	echo "queue ${nJobs}" >> ${instructionFile}

	condor_submit ${instructionFile}
	cd -
done

#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

for mask in 'nosrmask' 'srmask'
do
	#--------------------------------------------------#
	#---------------- INPUT PARAMETERS ----------------#
	mainInDir="/gpfs02/eic/anatochii/output_v6.3.1_${mask}_${beamEnergy}GeV_highStat" # input directory 
	treeName='tree'
	outputDir="/gpfs02/eic/anatochii/output_g4ana_v6.3.1_${mask}_${beamEnergy}GeV_highStat" # output directory
	buildDir="./build_${beamEnergy}GeV_${mask}" # build directory
	nJobs=10000 # number of jobs
	# in the custom-built Geant4 coordinate system
	minZ=-500.0 # [cm] minimum Z
	maxZ=500.0 # [cm] maximum Z
	eicshell='/direct/eic+u/anatochii/eic/eic-shell'
	r_target=20 # target radius to backtrack absorbed SR photons [mm]
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
	echo "echo \"./exe ${mainInDir} output_\${1}.root ${outputDir} output_ana_\${1} ${treeName} ${minZ} ${maxZ} ${r_target} &> ${outputDir}/logExe_\${1}.txt\" > exe_\${1}.bash" >> run.bash
	echo "chmod 777 exe_\${1}.bash" >> run.bash
	echo ". ${eicshell} -- ./exe_\${1}.bash" >> run.bash
	chmod 777 run.bash

	echo "executable = run.bash" > ${instructionFile}
	echo "arguments  = \$(ProcId)" >> ${instructionFile}
#	echo "log        = ${outputDir}/logJob_\$(ProcId).log" >> ${instructionFile}
#	echo "error      = ${outputDir}/logErr_\$(ProcId).err" >> ${instructionFile}
	echo "queue ${nJobs}" >> ${instructionFile}

	condor_submit ${instructionFile}
	cd -
done

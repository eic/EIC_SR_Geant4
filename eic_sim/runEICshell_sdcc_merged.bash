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
	inputFileName="/gpfs02/eic/anatochii/output_g4ana_v6.3.1_20240904_${beamEnergy}GeV_${runID}.hepmc"
	# output directory
	outputDirName="/gpfs02/eic/anatochii/output_eicsim_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
	# build directory
	buildDir="${outputDirName}/build_${runID}"
	# number of jobs
	nJobs=1
	# eic-shell
	eicshell='/direct/eic+u/anatochii/eic/eic-shell'
	# eic-setup
	eicsetup='/direct/eic+u/anatochii/eic/epic/install/bin/thisepic.sh'
	# eic compact file
	compactFile='/direct/eic+u/anatochii/eic/epic/install/share/epic/epic.xml'
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

	echo "executable = ${exeFile}" > ${instructionFile}
	echo "arguments  = \$(ProcId) ${inputFileName} ${outputDirName} ${eicshell} ${eicsetup} ${compactFile}" >> ${instructionFile}
	echo "log        = ${outputDirName}/logJob_\$(ProcId).log" >> ${instructionFile}
	echo "error      = ${outputDirName}/logErr_\$(ProcId).err" >> ${instructionFile}
	echo "queue ${nJobs}" >> ${instructionFile}

	condor_submit ${instructionFile}
	cd -
done

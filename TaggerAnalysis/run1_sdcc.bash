#!/bin/bash 

for beamEnergy in '18x275' #'10x275' '5x100'
do
	echo "[INFO] Beam Energy ${beamEnergy}GeV"

	eicshell='/direct/eic+u/anatochii/eic/eic-shell'
	# outputDir="/gpfs02/eic/anatochii/output_v6.3.1_${beamEnergy}GeV_LowQ2"
	# outputDir="/gpfs02/eic/anatochii/output_v6.3.1_${beamEnergy}GeV_Brems"
	outputDir="/gpfs02/eic/anatochii/output_v6.3.1_${beamEnergy}GeV_QuasiReal"
	fIn="${outputDir}/output_0.root"
	fOut="${outputDir}/ana1_output_0.root"
	runDir="${outputDir}/runDir"

	mkdir -pv ${runDir}
	cp ana1.C ${runDir}
	cd ${runDir}
	instructionFile="instr.sub"
	runFile='run.bash'
	exeFile='exe.bash'

	echo "#!/bin/bash" > ${runFile}
	echo "echo \"root -l -b -q ana1.C\(\\\\\\\"\$1\\\\\\\",\\\\\\\"\$2\\\\\\\"\) &> ${outputDir}/logExe_ana1.txt\" > ${exeFile}" >> ${runFile}  
	echo "chmod 777 ${exeFile}" >> ${runFile}
	echo ". ${eicshell} -- ./${exeFile}" >> ${runFile} 
	chmod 777 ${runFile} 

	echo "executable = ${runFile}" > ${instructionFile}
	echo "arguments  = ${fIn} ${fOut}" >> ${instructionFile}
	echo "log        = ${outputDir}/logJob_ana1.txt" >> ${instructionFile}
	echo "error      = ${outputDir}/logErr_ana1.txt" >> ${instructionFile}
	echo "queue ${nJobs}" >> ${instructionFile}

	condor_submit ${instructionFile}
	cd -
done

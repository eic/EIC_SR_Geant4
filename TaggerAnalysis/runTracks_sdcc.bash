#!/bin/bash 

for beamEnergy in '18x275' #'10x275' '5x100'
do
	echo "[INFO] Beam Energy ${beamEnergy}GeV"

	for dirName in 	'output_v6.3.1_18x275GeV_20241014_Pythia_farBWDmask' \
			'output_v6.3.1_18x275GeV_20241014_Pythia_nofarBWDmask' \
			'output_v6.3.1_18x275GeV_20241014_Brems_farBWDmask' \
			'output_v6.3.1_18x275GeV_20241014_Brems_nofarBWDmask' \
			'output_v6.3.1_18x275GeV_20241014_QuasiReal_nofarBWDmask' \
			'output_v6.3.1_18x275GeV_20241014_QuasiReal_farBWDmask'
	do
		echo "[INFO] Directory: ${dirName}"

		eicshell='/direct/eic+u/anatochii/eic/eic-shell'
		outputDir="/gpfs02/eic/anatochii/${dirName}"
		mkdir -pv pics_${dirName}
		cd pics_${dirName}
		cp ../plotTracks.C ./

		echo "root -l -b -q plotTracks.C\(\\\"${outputDir}\\\",\\\".root\\\",\\\"${outputDir}/tracks.root\\\",\\\"./\\\"\) &> ./log.txt" > exeTracks.sh
		chmod a+x exeTracks.sh

		echo "#!/bin/bash" > run.sh
		echo ". /direct/eic+u/anatochii/eic/eic-shell -- ./exeTracks.sh" >> run.sh
		chmod a+x run.sh

		instructionFile="instr.sub"
		echo "executable = run.sh" > ${instructionFile}
		echo "log = job.txt" >> ${instructionFile}
		echo "error = err.txt" >> ${instructionFile}
		echo "queue 1" >> ${instructionFile}

		condor_submit ${instructionFile}
		cd -
	done
done

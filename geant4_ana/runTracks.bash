#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

echo "[INFO] Beam Energy ${beamEnergy}GeV"

for runID in {1..100}
do
	echo ""
	echo "################################ RUNID = ${runID} ################################";

	directory="/work/eic/users/anatochii/output_g4sim_v6.3.1_20240906_${beamEnergy}GeV_${runID}"
	extension=".root"
	outputfile="${directory}_tracks.root"
	picsdir="${directory}/pics"

	mkdir -pv ${picsdir}

	time root -l -b -q plotTracks.C\(\"${directory}\",\"${extension}\",\"${outputfile}\",\"${picsdir}\"\)
done

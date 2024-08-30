#!/bin/bash

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

for mask in 'nosrmask' 'srmask5' 'srmask6'
do
	echo "[INFO] Beam Energy ${beamEnergy}GeV"

	directory="/gpfs02/eic/anatochii/output_v6.3.1_${mask}_${beamEnergy}GeV_tracks"
	extension=".root"
	outputfile="${directory}.root"
	picsdir="${directory}/pics"

	mkdir -pv ${picsdir}

	time root -l -b -q plotTracks.C\(\"${directory}\",\"${extension}\",\"${outputfile}\",\"${picsdir}\"\)
done

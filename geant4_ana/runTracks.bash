#!/bin/bash

for beamEnergy in '18x275' '10x275' '5x100'
do
	directory="/gpfs02/eic/anatochii/output_v6.3.1_nosrmask_${beamEnergy}GeV_lowStat"
	extension=".root"
	outputfile="/gpfs02/eic/anatochii/output_v6.3.1_nosrmask_${beamEnergy}GeV_lowStat_tracks.root"
	picsdir="pics_v6.3.1_nosrmask_${beamEnergy}GeV_lowStat"

	mkdir -pv ${picsdir}

	time root -l -b -q plotTracks.C\(\"${directory}\",\"${extension}\",\"${outputfile}\",\"${picsdir}\"\)
done

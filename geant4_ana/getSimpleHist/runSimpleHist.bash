#!/bin/bash

# activate conda
source /gpfs/mnt/gpfs02/eic/anatochii/miniforge3/bin/activate

for beamEnergy in '18x275' '10x275' '5x100'
do
	directory="/gpfs02/eic/anatochii/output_g4ana_v6.3.1_nosrmask_${beamEnergy}GeV_lowStat"
	nFiles=1000
	mkdir -pv log

	time root -l -b -q getSimpleHisto.C\(\"${directory}\",${nFiles}\) >& ./log/log_output_g4ana_v6.3.1_nosrmask_${beamEnergy}GeV_lowStat.txt
done

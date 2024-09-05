#!/bin/bash

date 

beamEnergy=$1 # '18x275' or '10x275' or '5x100'
runID=$2

echo "[INFO] Beam Energy ${beamEnergy}GeV"
echo "[INFO] RunID = ${runID}"

# activate conda
source /gpfs/mnt/gpfs02/eic/anatochii/miniforge3/bin/activate ""

directory="/gpfs02/eic/anatochii/output_g4ana_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
nFiles=10000
mkdir -pv log

time root -l -b -q getSimpleHisto.C\(\"${directory}\",${nFiles}\) >& ./log/log_output_g4ana_v6.3.1_20240904_${beamEnergy}GeV_${runID}.txt

date

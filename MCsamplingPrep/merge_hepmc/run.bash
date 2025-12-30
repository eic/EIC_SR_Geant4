#!/bin/bash

workDir=$PWD
eicshell='/home/natochii/eic/eic-shell'

nInputFileRepeat=10000 #runsPerJob * nJobs

#---- run001
fileName='dataprod_rel_1.0.0_synrad_18x275_run001'

#---- 5GeV
#outputDir='/work/eic//users/anatochii/MCsamplingPrep_18GeV_scaled_to_5GeV'
#outFileName="dataprod_rel_1.0.0_synrad_5x100_run001.preproc_${nInputFileRepeat}repeats.hepmc3"

#---- 10GeV
outputDir='/work/eic/users/anatochii/MCsamplingPrep_18GeV_scaled_to_10GeV_10um_EMZ'
outFileName="dataprod_rel_1.0.0_synrad_10x275_run001.preproc_${nInputFileRepeat}repeats.hepmc3"

mkdir -pv ${outputDir}/buildMergeHepMC
cd ${outputDir}/buildMergeHepMC

${eicshell} << EOF
cmake ${workDir}
make -j2;
time ./exe ${outputDir}/${outFileName} ${outputDir}/${fileName}_ ${nInputFileRepeat} ".preproc.hepmc3" &> log.txt
EOF
cd -

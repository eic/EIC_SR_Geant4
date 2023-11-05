#!/bin/bash

# output directory
mkdir -pv output

# build directory
mkdir -pv build
cd build

# clean
make clean

# config
cmake ../

# compile
make -j4

for sample in 'noEcut' '5keVcut'
do
	mainInDir="/group/belle2/TMP/anatochi/eic/output_${sample}"
	outputFileName="output_${sample}"

	inputFileName="output_*.root"
	mainOutDir="/group/belle2/TMP/anatochi/eic/output_ana"
	treeName="tree"

	mkdir -pv ${mainOutDir}

	com="./exe ${mainInDir} ${inputFileName} ${mainOutDir} ${outputFileName} ${treeName} &> ${mainOutDir}/log_${sample}.txt"
	echo -ne "[INFO] ${com} => "
	bsub -q $queue -o /dev/null "${com}"
done

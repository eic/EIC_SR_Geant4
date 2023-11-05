#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
mainInDir='/group/belle2/TMP/anatochi/eic/output' # input directory
inputFileName='output_*.root'
treeName='tree'
outputDir='/group/belle2/TMP/anatochi/eic/output_ana' # output directory
outputFileName='output'
buildDir='./build' # build directory
queue='s'
#--------------------------------------------------#
#--------------------------------------------------#

# output directory
mkdir -pv ${outputDir}

# build directory
mkdir -pv ${buildDir}
cd ${buildDir}

# config
cmake ../

# compile
make -j4

com="./exe ${mainInDir} ${inputFileName} ${outputDir} ${outputFileName} ${treeName} &> ${outputDir}/logExe_ana.txt"
echo -ne "[INFO] ${com} => "
bsub -q $queue -o ${outputDir}/logJob_ana.txt "${com}"

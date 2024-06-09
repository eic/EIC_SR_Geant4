#!/bin/bash 
queue='s'

# TaggerScan1
<<CC
outputDir='/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_TaggerScan1_ChamberH_10cm'
for PosZ in {-995..995..5}
do
	fIn="${outputDir}/output_${PosZ}.root"
	fOut="${outputDir}/ana1_output_${PosZ}.root"
	bsub -q $queue -o ${outputDir}/log_ana1_${PosZ}.txt "root -l -b -q ana1.C\(\\\"${fIn}\\\",\\\"${fOut}\\\"\)"
done
CC

# AnteChamberScan
outputDir='/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_Tagger1_-197.2cm_ChamberHscan'
for aH in {1..10..1}
do
	fIn="${outputDir}/output_${aH}.root"
	fOut="${outputDir}/ana1_output_${aH}.root"
	bsub -q $queue -o ${outputDir}/log_ana1_${aH}.txt "root -l -b -q ana1.C\(\\\"${fIn}\\\",\\\"${fOut}\\\"\)"
done


#!/bin/bash 
queue='sx'

<<CC
# TaggerScan1
outputDir='/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_TaggerScan1_ChamberH_10cm'
for PosZ in {-995..995..5}
do
	fIn="${outputDir}/output_${PosZ}.root"
	fOut="${outputDir}/ana1_output_${PosZ}.root"
	bsub -q $queue -o ${outputDir}/log_ana1_${PosZ}.txt "root -l -b -q ana1.C\(\\\"${fIn}\\\",\\\"${fOut}\\\"\)"
done
CC
<<DD
# AnteChamberScan
outputDir='/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_Tagger1_-150.0cm_ChamberHscan'
for aH in {1..10..1}
do
	fIn="${outputDir}/output_${aH}.root"
	fOut="${outputDir}/ana1_output_${aH}.root"
	bsub -q $queue -o ${outputDir}/log_ana1_${aH}.txt "root -l -b -q ana1.C\(\\\"${fIn}\\\",\\\"${fOut}\\\"\)"
done
DD
<<FF
outputDir='/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_TaggerScan2_ChamberH_4cm'
for PosZ in {-995..995..5}
do
	fIn="${outputDir}/output_${PosZ}.root"
	fOut="${outputDir}/ana1_output_${PosZ}.root"
	bsub -q $queue -o ${outputDir}/log_ana1_${PosZ}.txt "root -l -b -q ana1.C\(\\\"${fIn}\\\",\\\"${fOut}\\\"\)"
done
FF
<<KK
outputDir='/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_Tagger1_-171.8cm_ChamberH_4cm'
PosZ=-171.8 # cm
fIn="${outputDir}/output_${PosZ}.root"
fOut="${outputDir}/ana1_output_${PosZ}.root"
bsub -q $queue -o ${outputDir}/log_ana1_${PosZ}.txt "root -l -b -q ana1.C\(\\\"${fIn}\\\",\\\"${fOut}\\\"\)"
KK
<<LL
outputDir='/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_nosrmask_Tagger1_-150cm_ChamberH_6cm'
PosZ=-150 # cm
fIn="${outputDir}/output_${PosZ}.root"
fOut="${outputDir}/ana1_output_${PosZ}.root"
bsub -q $queue -o ${outputDir}/log_ana1_${PosZ}.txt "root -l -b -q ana1.C\(\\\"${fIn}\\\",\\\"${fOut}\\\"\)"
LL

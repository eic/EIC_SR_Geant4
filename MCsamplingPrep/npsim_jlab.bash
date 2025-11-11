#!/bin/bash

workDir=$PWD
compactFile="${workDir}/epic_ip6_cut_25.01.1.xml"
eicshell='/home/natochii/eic/eic-shell'
eicconfig='/home/natochii/eic/epic_for_MCsamplingPrep/install/bin/thisepic.sh'

#---- run001
inputDir='/work/eic/users/anatochii/SynradG4_HepMC_Files_SR_on_IP6/data/synrad/dataprod_rel_1.0.0/18x275'
fileName='dataprod_rel_1.0.0_synrad_18x275_run001'

#---- 5GeV
#outputDir='/work/eic//users/anatochii/MCsamplingPrep_18GeV_scaled_to_5GeV'
#PrimaryParticleEnergyCut_eV=30000 # [eV]

#---- 10GeV
outputDir='/work/eic/users/anatochii/MCsamplingPrep_18GeV_scaled_to_10GeV'
PrimaryParticleEnergyCut_eV=200000 # [eV]

nJobs=1000
runsPerJob=10
# nInputFileRepeat = runsPerJob * nJobs

echo "[INFO] inputDir = ${inputDir}"
echo "[INFO] outputDir = ${outputDir}"
echo "[INFO] fileName = ${fileName}"
echo "[INFO] compactFile = ${compactFile}"
echo "[INFO] nJobs = ${nJobs}"
echo "[INFO] runsPerJob = ${runsPerJob}"

mkdir -pv ${outputDir}/build
cd ${outputDir}/build/

echo "#!/bin/bash" 						 > exe.bash
echo "# Check input" 						>> exe.bash
echo "block=\$1" 						>> exe.bash
echo "mkdir -pv run_\${block}"					>> exe.bash
echo "cd run_\${block}"						>> exe.bash
echo "block_size=${runsPerJob}" 				>> exe.bash
echo "minFileID=\$(( (block - 1) * block_size + 1 ))" 		>> exe.bash
echo "maxFileID=\$(( block * block_size ))" 			>> exe.bash
echo "echo \"[INFO] Range: \${minFileID} - \${maxFileID}\""	>> exe.bash
echo "${eicshell} << EOF" 					>> exe.bash
echo "source ${eicconfig}" 					>> exe.bash 
echo "export LD_LIBRARY_PATH=${workDir}/SteppingPlugin/install/lib:\\\${LD_LIBRARY_PATH}" >> exe.bash
echo "echo \"[INFO] LD_LIBRARY_PATH = \\\${LD_LIBRARY_PATH}\"" 	>> exe.bash
echo "for (( i=\${minFileID}; i<=\${maxFileID}; i++ ))" 	>> exe.bash 
echo "do" >> exe.bash
echo "	inputFile=\"${inputDir}/${fileName}.hepmc3.tree.root\"" 		>> exe.bash 
echo "	outputFile1=\"${outputDir}/${fileName}_\\\${i}.edm4hep.root\"" 		>> exe.bash 
echo "	outputFile2=\"${outputDir}/${fileName}_\\\${i}.preproc.hepmc3\"" 	>> exe.bash 
echo "	json_string=\\\"{\\\\\\\"name\\\\\\\":\\\\\\\"MySteppingAction\\\\\\\",\\\\\\\"parameter\\\\\\\":{\\\\\\\"OutputFileNameWithSteppingInfo\\\\\\\":\\\\\\\"\\\\\\\${outputFile2}\\\\\\\",\\\\\\\"PrimaryParticleEnergyCut_eV\\\\\\\":${PrimaryParticleEnergyCut_eV}}}\\\"" >> exe.bash 
echo "	echo \"[INFO] inputFile = \\\${inputFile}\"" 				>> exe.bash 
echo "	echo \"[INFO] outputFile1 = \\\${outputFile1}\"" 			>> exe.bash 
echo "	echo \"[INFO] outputFile2 = \\\${outputFile2}\""			>> exe.bash 
echo "	echo \"[INFO] json_string = \\\${json_string}\"" 			>> exe.bash 
#echo "	time npsim --action.step \\\$json_string --runType batch --numberOfEvents -1 --compactFile ${compactFile} --outputFile \\\${outputFile1} --physics.list FTFP_BERT --inputFiles \\\${inputFile}" >> exe.bash 
echo "	time npsim --action.step \\\$json_string --runType batch --numberOfEvents -1 --compactFile ${compactFile} --outputFile \\\${outputFile1} --physics.list FTFP_BERT_EMZ --inputFiles \\\${inputFile}" >> exe.bash 
echo "done" 				>> exe.bash
echo "EOF" 				>> exe.bash
echo "cd ..; rm -rf run_\${block}"	>> exe.bash

chmod a+x exe.bash

# submit jobs
instructionFile="instr.sub"
echo "#!/bin/bash" > ${instructionFile}
echo "#" >> ${instructionFile}
echo "#SBATCH --job-name=MCsamplingPrep" >> ${instructionFile}
echo "#SBATCH --nodes=1" >> ${instructionFile}
echo "#SBATCH --ntasks=1" >> ${instructionFile}
echo "#SBATCH --cpus-per-task=1" >> ${instructionFile}
echo "#SBATCH --partition=production" >> ${instructionFile}
echo "#SBATCH --mem-per-cpu=4G" >> ${instructionFile} # memory per cpu-core
echo "#SBATCH --time=02:00:00" >> ${instructionFile}
echo "#SBATCH --output=jobLog_%A_%a_%j.txt" >> ${instructionFile}
echo "#SBATCH --error=errLog_%A_%a_%j.txt" >> ${instructionFile}
echo "#SBATCH --array=1-${nJobs}" >> ${instructionFile}
echo "" >> ${instructionFile} 
echo "./exe.bash \${SLURM_ARRAY_TASK_ID} &> exeLog_\${SLURM_ARRAY_TASK_ID}.txt" >> ${instructionFile}

sbatch ${instructionFile}
cd -

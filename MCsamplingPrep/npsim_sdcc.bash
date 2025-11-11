#!/bin/bash

workDir=$PWD
compactFile="${workDir}/epic_ip6_cut_25.01.1.xml"
eicshell='/gpfs02/eic/anatochii/eic/eic-shell'
eicconfig='/gpfs02/eic/anatochii/eic/epic/install/bin/thisepic.sh'

#---- run001
inputDir='/gpfs02/eic/anatochii/SynradG4_HepMC_Files_SR_on_IP6/data/synrad/dataprod_rel_1.0.0/18x275'
fileName='dataprod_rel_1.0.0_synrad_18x275_run001'

#---- 5GeV
#outputDir='/gpfs02/eic/anatochii/MCsamplingPrep_18GeV_scaled_to_5GeV'
#PrimaryParticleEnergyCut_eV=30000 # [eV]

#---- 10GeV
outputDir='/gpfs02/eic/anatochii/MCsamplingPrep_18GeV_scaled_to_10GeV'
PrimaryParticleEnergyCut_eV=200000 # [eV]

nInputFileRepeat=10000

echo "[INFO] inputDir = ${inputDir}"
echo "[INFO] outputDir = ${outputDir}"
echo "[INFO] fileName = ${fileName}"
echo "[INFO] compactFile = ${compactFile}"
echo "[INFO] nInputFileRepeat = ${nInputFileRepeat}"

mkdir -pv ${outputDir}/build
cd ${outputDir}/build/
dir=$(pwd)

echo "#!/bin/bash" > exe.bash
echo "iJob=\$1" >> exe.bash
echo "${eicshell} << EOF" >> exe.bash
echo "cd ${dir}" >> exe.bash
echo "source ${eicconfig}" >> exe.bash 
echo "export LD_LIBRARY_PATH=${workDir}/SteppingPlugin/install/lib:\\\${LD_LIBRARY_PATH}" >> exe.bash
echo "echo \"[INFO] LD_LIBRARY_PATH = \\\${LD_LIBRARY_PATH}\"" >> exe.bash
echo "inputFile=\"${inputDir}/${fileName}.hepmc3.tree.root\"" >> exe.bash 
echo "outputFile1=\"${outputDir}/${fileName}_\${iJob}.edm4hep.root\"" >> exe.bash 
echo "outputFile2=\"${outputDir}/${fileName}_\${iJob}.preproc.hepmc3\"" >> exe.bash 
echo "json_string=\"{\\\"name\\\":\\\"MySteppingAction\\\",\\\"parameter\\\":{\\\"OutputFileNameWithSteppingInfo\\\":\\\"\\\${outputFile2}\\\",\\\"PrimaryParticleEnergyCut_eV\\\":${PrimaryParticleEnergyCut_eV}}}\"" >> exe.bash 
echo "echo \"[INFO] inputFile = \\\${inputFile}\"" >> exe.bash 
echo "echo \"[INFO] outputFile1 = \\\${outputFile1}\"" >> exe.bash 
echo "echo \"[INFO] outputFile2 = \\\${outputFile2}\"">> exe.bash 
echo "echo \"[INFO] json_string = \\\${json_string}\"" >> exe.bash 
echo "time npsim --action.step \\\$json_string --runType batch --numberOfEvents -1 --compactFile ${compactFile} --outputFile \\\${outputFile1} --physics.list FTFP_BERT --inputFiles \\\${inputFile}" >> exe.bash 
echo "EOF" >> exe.bash

chmod a+x exe.bash

# submit jobs
echo "executable	= exe.bash" > instr.sub
echo "arguments		= \$(ProcId)" >> instr.sub 
echo "log		= logJob_\$(ProcId).log" >> instr.sub
echo "error		= logErr_\$(ProcId).err" >> instr.sub
echo "output		= logOut_\$(ProcId).out" >> instr.sub
echo "queue ${nInputFileRepeat}" >> instr.sub

condor_submit instr.sub
cd -

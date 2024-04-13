#!/bin/bash

jobID=$1
inputDirName=$2
outputDirName=$3
eicshell=$4
eicsetup=$5
compactFile=$6

runDir="runEICsim_${jobID}"
mkdir -pv ${runDir} 
cd ${runDir}

runFile="runEICsim.sh"
echo "#!/bin/bash" > ${runFile}
echo "source ${eicsetup}" >> ${runFile} 
echo -ne "time npsim --runType batch --numberOfEvents -1 --compactFile ${compactFile}" >> ${runFile}
echo " --inputFiles ${inputDirName}/output_ana_${jobID}.hepmc --outputFile ${outputDirName}/output_eicsim_${jobID}.edm4hep.root &> ${outputDirName}/logExe_${jobID}.txt" >> ${runFile}

chmod 777 ${runFile} 

. ${eicshell} -- ./${runFile}

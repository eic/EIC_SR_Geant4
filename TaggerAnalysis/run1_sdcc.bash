#!/bin/bash 

eicshell='/direct/eic+u/anatochii/eic/eic-shell'
outputDir='/gpfs02/eic/anatochii/output_v6.3_modif_bwd_hepmc'
fIn="${outputDir}/output_0.root"
fOut="${outputDir}/ana1_output_0.root"
runDir="./runDir"

mkdir -pv ${outputDir}
mkdir -pv ${runDir}
cd ${runDir}
cp ../ana1.C ./
instructionFile="instr.sub"
runFile='run.bash'
exeFile='exe.bash'

echo "#!/bin/bash" > ${runFile}
echo "echo \"root -l -b -q ana1.C\(\\\\\\\"\$1\\\\\\\",\\\\\\\"\$2\\\\\\\"\) &> ${outputDir}/logExe_ana1.txt\" > ${exeFile}" >> ${runFile}  
echo "chmod 777 ${exeFile}" >> ${runFile}
echo ". ${eicshell} -- ./${exeFile}" >> ${runFile} 
chmod 777 ${runFile} 

echo "executable = ${runFile}" > ${instructionFile}
echo "arguments  = ${fIn} ${fOut}" >> ${instructionFile}
echo "log        = ${outputDir}/logJob_ana1.txt" >> ${instructionFile}
echo "error      = ${outputDir}/logErr_ana1.txt" >> ${instructionFile}
echo "queue ${nJobs}" >> ${instructionFile}

condor_submit ${instructionFile}

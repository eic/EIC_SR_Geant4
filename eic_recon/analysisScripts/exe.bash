#!/bin/bash

jobID=$1
inputDirName=$2
outputDirName=$3
eicshell=$4
eicsetup=$5
scriptName=$6

runFile="runGetAllHits_${jobID}.sh"
echo "#!/bin/bash" > ${runFile}
echo "source ${eicsetup}" >> ${runFile} 

echo -ne "time root -l -b -q ${scriptName}\(" >> ${runFile}
echo -ne "\\\"${inputDirName}/output_eicrecon_${jobID}.podio.root\\\"" >> ${runFile}
echo -ne "," >> ${runFile}
echo -ne "\\\"${outputDirName}/output_eicrecon_${jobID}.hist.root\\\"" >> ${runFile}
echo     "\)" >> ${runFile}

# make the file executable
chmod 777 ${runFile}

. ${eicshell} -- ./${runFile}

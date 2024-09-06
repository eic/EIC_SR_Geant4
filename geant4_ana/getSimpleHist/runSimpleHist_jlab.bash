#!/bin/bash

date 

beamEnergy=$1 # '18x275' or '10x275' or '5x100'

echo "[INFO] Beam Energy ${beamEnergy}GeV"

# activate conda
source /work/eic/users/anatochii/miniforge3/bin/activate ""

scriptName='getSimpleHisto.C'

nFiles=10000
mkdir -pv runDir
cd runDir
cp ../${scriptName} ./

for runID in {1..10}
do
	echo "[INFO] RunID: ${runID}"

	instructionFile="instrJob_${beamEnergy}_${runID}.sh"
	directory="/work/eic/users/anatochii/output_g4ana_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
	logFile="log_output_g4ana_v6.3.1_20240904_${beamEnergy}GeV_${runID}.txt"

	echo "#!/bin/bash" > ${instructionFile}
	echo "#" >> ${instructionFile}
	echo "#SBATCH --job-name=Get-hist" >> ${instructionFile}
	echo "#SBATCH --nodes=1" >> ${instructionFile}
	echo "#SBATCH --ntasks=1" >> ${instructionFile}
	echo "#SBATCH --cpus-per-task=1" >> ${instructionFile}
	#echo "#SBATCH --mem=1G" >> ${instructionFile} # total memory per node
	echo "#SBATCH --mem-per-cpu=8G" >> ${instructionFile} # memory per cpu-core
	echo "#SBATCH --partition=production" >> ${instructionFile}
	echo "#SBATCH --time=03:00:00" >> ${instructionFile}
	echo "" >> ${instructionFile} 

	echo "time root -l -b -q ${scriptName}\(\\\"${directory}\\\",${nFiles}\) >& ${logFile}" >> ${instructionFile}
	echo "date >> ${logFile}" >> ${instructionFile}

	sbatch ${instructionFile}
done
cd -

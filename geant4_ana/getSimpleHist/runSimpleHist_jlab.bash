#!/bin/bash

date 

beamEnergy=$1 # '18x275' or '10x275' or '5x100'
nRuns=10

echo "[INFO] Beam Energy ${beamEnergy}GeV"
echo "[INFO] RunID = ${nRuns}"

# activate conda
source /work/eic/users/anatochii/miniforge3/bin/activate ""

directory="/work/eic/users/anatochii/output_g4ana_v6.3.1_20240904_${beamEnergy}GeV_${runID}"
nFiles=10000
mkdir -pv log

instructionFile="instrJob_${beamEnergy}.sh"

echo "#!/bin/bash" > ${instructionFile}
echo "#" >> ${instructionFile}
echo "#SBATCH --job-name=Get-hist" >> ${instructionFile}
echo "#SBATCH --nodes=1" >> ${instructionFile}
echo "#SBATCH --ntasks=1" >> ${instructionFile}
echo "#SBATCH --cpus-per-task=1" >> ${instructionFile}
#echo "#SBATCH --mem=1G" >> ${instructionFile} # total memory per node
echo "#SBATCH --mem-per-cpu=8G" >> ${instructionFile} # memory per cpu-core
echo "#SBATCH --partition=production" >> ${instructionFile}
echo "#SBATCH --time=02:00:00" >> ${instructionFile}
echo "#SBATCH -o /dev/null" >> ${instructionFile}
echo "#SBATCH --array=1-${nRuns}" >> ${instructionFile}
echo "" >> ${instructionFile} 

echo "directory=\"/work/eic/users/anatochii/output_g4ana_v6.3.1_20240904_${beamEnergy}GeV_\${SLURM_ARRAY_TASK_ID}\"" >> ${instructionFile}
echo "time root -l -b -q getSimpleHisto.C\(\\\"\${directory}\\\",${nFiles}\) >& ./log/log_output_g4ana_v6.3.1_20240904_${beamEnergy}GeV_\${SLURM_ARRAY_TASK_ID}.txt" >> ${instructionFile}
echo "date >> ./log/log_output_g4ana_v6.3.1_20240904_${beamEnergy}GeV_\${SLURM_ARRAY_TASK_ID}.txt" >> ${instructionFile}

sbatch ${instructionFile}

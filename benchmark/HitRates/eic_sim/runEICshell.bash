#!/bin/bash


#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
outputDirName='/gpfs/group/belle2/TMP/anatochi/eic/output_eicsim_synrad_new_sampling_highstat' # output directory
buildDir='./build' # build directory
queue='hx'
eicshell='/home/belle2/anatochi/eic/eic-shell'
#--------------------------------------------------#
#--------------------------------------------------#

echo "[INFO] outputDirName = ${outputDirName}"
echo "[INFO] buildDir = ${buildDir}"
echo "[INFO] queue = ${queue}"

# output directory
mkdir -pv ${outputDirName}

# build directory
mkdir -pv ${buildDir}
cd ${buildDir}

# submit a job
runFile="runEICsim.sh"
echo "#!/bin/bash" > ${runFile}

# echo "source /opt/detector/setup.sh" >> ${runFile}
# echo -ne "time npsim --runType batch --numberOfEvents -1 --compactFile /opt/detector/epic-nightly/share/epic/epic.xml" >> ${runFile}

echo "source /home/belle2/anatochi/eic/epic/install/setup.sh" >> ${runFile}
echo -ne "time npsim --runType batch --numberOfEvents -1 --compactFile /home/belle2/anatochi/eic/epic/install/share/epic/epic.xml" >> ${runFile}

echo " --inputFiles ../SR_out_int_window_10.0fs_nevents_10000_pid_22_status_4_1_seed_1.hepmc --outputFile ${outputDirName}/output_eicsim.edm4hep.root &> ${outputDirName}/logExe_eicsim.txt" >> ${runFile}

chmod 777 ${runFile}
bsub -q $queue -o ${outputDirName}/logJob_eicsim.txt ". ${eicshell} -- ./${runFile}"

cd -

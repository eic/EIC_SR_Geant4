#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
inputDirName='/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_eicrecon_v6.2_srmask5_1_ip_cut2' # output directory
buildDir='./build2' # build directory
nJobs=10000 # number of jobs
queue='s'
eicshell='/home/belle2/anatochi/eic/eic-shell'
# eicsetup='/opt/detector/setup.sh'
eicsetup='/home/belle2/anatochi/eic/epic/install/setup.sh'
scriptName='getAllVerticesPerFile.C'
#--------------------------------------------------#
#--------------------------------------------------#

echo "[INFO] inputDirName = ${inputDirName}"
echo "[INFO] buildDir = ${buildDir}"
echo "[INFO] nJobs = ${nJobs}"
echo "[INFO] queue = ${queue}"
echo "[INFO] scriptName = ${scriptName}"

# build directory
mkdir -pv ${buildDir}
cd ${buildDir}
cp ../${scriptName} ./

runID=0
runFile="runGetAllVertices_${runID}.sh"
echo "#!/bin/bash" > ${runFile}
echo "source ${eicsetup}" >> ${runFile} 

# loop over all jobs
for jobID in $(seq 1 1 ${nJobs})
do
	# submit a job
	echo -ne "time root -l -b -q ${scriptName}\(" >> ${runFile}
	echo -ne "\\\"${inputDirName}/output_eicrecon_${jobID}.podio.root\\\"" >> ${runFile}
	echo -ne "," >> ${runFile}
	echo -ne "\\\"${inputDirName}/output_eicrecon_${jobID}.vertex.root\\\"" >> ${runFile}
	echo     "\)" >> ${runFile}

	jobID_tmp=$(($jobID % 10))
	if [ $jobID_tmp -eq 0 ]; then
		# make the file executable
		chmod 777 ${runFile}
		
		# submit a job
		echo -ne "Run = ${runID} (last job = ${jobID}/${nJobs}): "
		bsub -q $queue -o ${inputDirName}/logJob_vertex_${jobID}.txt ". ${eicshell} -- ./${runFile}"

		# move to another run
		runID=$(($runID+1))
		runFile="runEICrecon_${runID}.sh"
		echo "#!/bin/bash" > ${runFile}
		echo "source ${eicsetup}" >> ${runFile}
	fi
done

cd -

echo "When '${queue}' jobs finished, run in eic-shell: root -l -b -q drawAllVertices.C\\(\\\"${inputDirName}\\\",\\\".vertex\\\",\\\".root\\\",\\\"./pics\\\"\\)"


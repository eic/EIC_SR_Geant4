#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
inputDirName='/gpfs/group/belle2/TMP/anatochi/eic/output_eicsim' # input directory
outputDirName='/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon' # output directory
buildDir='./build' # build directory
nJobs=1000 # number of jobs
queue='s'
eicshell='/home/belle2/anatochi/eic/eic-shell'
# eicsetup='/opt/detector/setup.sh'
eicsetup='/home/belle2/anatochi/eic/epic/install/setup.sh'
#--------------------------------------------------#
#--------------------------------------------------#

echo "[INFO] inputDirName = ${inputDirName}"
echo "[INFO] outputDirName = ${outputDirName}"
echo "[INFO] buildDir = ${buildDir}"
echo "[INFO] nJobs = ${nJobs}"
echo "[INFO] queue = ${queue}"

#-------------------------------------------------------------#
#---------- Compile the custom analysis plugin ---------------#
#<<COMMENT
	rm -rf analysisPlugin/build
	rm -rf EICrecon_MY
	mkdir -pv EICrecon_MY

	compileScript='compileAnalysisPlugin.bash'
	echo "#!/bin/bash" > ${compileScript}
	echo "source ${eicsetup}" >> ${compileScript}
	echo "export EICrecon_MY=${PWD}/EICrecon_MY" >> ${compileScript}
	echo "cmake -S analysisPlugin -B analysisPlugin/build" >> ${compileScript}
	echo "cmake --build analysisPlugin/build --target install" >> ${compileScript}

	chmod 777 ${compileScript}
	. $eicshell -- ./${compileScript}
	rm -rf ${compileScript}
#COMMENT
#-------------------------------------------------------------#
#-------------------------------------------------------------#

# output directory
mkdir -pv ${outputDirName}

# build directory
mkdir -pv ${buildDir}
cd ${buildDir}

for jobID in $(seq 1 1 ${nJobs})
do
	# count how many jobs are pending, the maximum is 20000 jobs
	nPend=$(bjobs | grep "PEND" | wc -l);
	while [ $nPend -gt 19000 ]
	do
		date
		echo "[INFO] Waiting 10min for an empty slot to submit the next job to an LSF queue ..."
		sleep 600
		nPend=$(bjobs | grep "PEND" | wc -l)
	done

	# submit a job
	runFile="runEICrecon_${jobID}.sh"
	echo "#!/bin/bash" > ${runFile}
	echo "source ${eicsetup}" >> ${runFile}
	echo "export EICrecon_MY=${PWD}/../EICrecon_MY" >> ${runFile}
	echo -ne "time eicrecon -Pplugins=analysisPlugin -Ppodio:output_include_collections=\"MCParticles,VertexBarrelHits,TrackerEndcapHits,SiBarrelHits\" " >> ${runFile} 
	echo -ne "-Ppodio:output_file=${outputDirName}/output_eicrecon_${jobID}.podio.root " >> ${runFile} 
	echo "-Phistsfile=${outputDirName}/output_eicrecon_${jobID}.hist.root ${inputDirName}/output_eicsim_${jobID}.edm4hep.root" >> ${runFile}
	# -Pjana:nevents=1000 <-- to limit the number of events

	chmod 777 ${runFile}
	bsub -q $queue -o ${outputDirName}/logJob_eicrecon_${jobID}.txt ". ${eicshell} -- ./${runFile}"
done

cd -


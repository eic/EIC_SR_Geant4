#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
outputDirName='/gpfs/group/belle2/TMP/anatochi/eic/output_matscan' # output directory
buildDir='./build' # build directory
nJobs=2001
queue='s'
eicshell='/home/belle2/anatochi/eic/eic-shell'
scriptName='g4MaterialScan_to_csv'
#--------------------------------------------------#
#--------------------------------------------------#

echo "[INFO] outputDirName = ${outputDirName}"
echo "[INFO] buildDir = ${buildDir}"
echo "[INFO] scriptName = ${scriptName}"
echo "[INFO] nJobs = ${nJobs}"
echo "[INFO] queue = ${queue}"

# build directory
rm -rf ${buildDir}
mkdir -pv ${buildDir}

cp ${scriptName} ${buildDir}/
cd ${buildDir}

# output directory
mkdir -pv ${outputDirName}

#---------------------#
# XZ-plane scan
xx=-4000.0 # [mm]
ymin=-10.0 # [mm]
ymax=10.0 # [mm]
dy=1.0 # [mm]
zz=-8000.0 # [mm]
dz=8.0 # [mm]
etamin=0
etamax=0
etastep=1
phimin=0 # [deg]
phimax=0 # [deg]
phistep=1 # [deg]
#---------------------#

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
	runFile="runScanJob_${jobID}.sh"
	echo "#!/bin/bash" > ${runFile}
	echo "source /opt/detector/epic-nightly/setup.sh" >> ${runFile}

	yy=${ymin}
	while [ "$(bc <<< "${yy} <= ${ymax}")" == "1"  ]
	do
		echo -ne "time python3 ${scriptName}" >> ${runFile}
		echo -ne " --compact=/opt/detector/epic-nightly/share/epic/epic.xml" >> ${runFile}
		echo -ne " --output=${outputDirName}/g4mat_scan_${xx}_${yy}_${zz}.csv" >> ${runFile}
		echo -ne " --start-point=${xx},${yy},${zz}" >> ${runFile}
		echo -ne " --eta=${etamin}:${etamax}:${etastep}" >> ${runFile}
		echo -ne " --phi ${phimin}:${phimax}:${phistep}" >> ${runFile}
		echo -ne "" >> ${runFile}
		echo " &> ${outputDirName}/logExe_matscan_${xx}_${yy}_${zz}.txt" >> ${runFile}
		yy=`echo $yy + $dy | bc`
	done

	chmod 777 ${runFile}
	echo -ne "${jobID} /  ${nJobs} : "
	#bsub -q $queue -o ${outputDirName}/logJob_matscan_${jobID}.txt ". ${eicshell} -- ./${runFile}"

	zz=`echo $zz + $dz | bc`
done
cd -

#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
inputDirName='/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_eicsim_v6.2_srmask5_1_ip_cut3' # input directory
outputDirName='/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_eicrecon_v6.2_srmask5_1_ip_cut3' # output directory
buildDir='./build' # build directory
nJobs=10000 # number of jobs
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

# output directory
mkdir -pv ${outputDirName}

# build directory
mkdir -pv ${buildDir}
cd ${buildDir}

runID=0
runFile="runEICrecon_${runID}.sh"
echo "#!/bin/bash" > ${runFile}
echo "source ${eicsetup}" >> ${runFile} 

nHitsArray=("B0ECalHits" "B0TrackerHits" "BackwardMPGDEndcapHits" "DIRCBarHits" "DRICHHits" "EcalBarrelImagingHits" "EcalBarrelScFiHits" "EcalEndcapNHits" "EcalEndcapPHits" "EcalEndcapPInsertHits" "EcalFarForwardZDCHits" "EcalLumiSpecHits" "ForwardMPGDEndcapHits" "ForwardOffMTrackerHits" "ForwardRomanPotHits" "HcalBarrelHits" "HcalEndcapNHits" "HcalEndcapPInsertHits" "HcalFarForwardZDCHits" "LFHCALHits" "LumiDirectPCALHits" "LumiSpecTrackerHits" "MPGDBarrelHits" "OuterMPGDBarrelHits" "PFRICHHits" "SiBarrelHits" "TaggerTrackerHits" "TOFBarrelHits" "TOFEndcapHits" "TrackerEndcapHits" "VertexBarrelHits")

nRawHitsArray=("B0ECalRawHits" "B0TrackerRawHits" "BackwardMPGDEndcapRawHits" "DIRCRawHits" "DRICHRawHits" "EcalBarrelImagingRawHits" "EcalBarrelScFiRawHits" "EcalEndcapNRawHits" "EcalEndcapPRawHits" "EcalEndcapPInsertRawHits" "EcalFarForwardZDCRawHits" "EcalLumiSpecRawHits" "ForwardMPGDEndcapRawHits" "ForwardOffMTrackerRawHits" "ForwardRomanPotRawHits" "HcalBarrelRawHits" "HcalEndcapNRawHits" "HcalEndcapPInsertRawHits" "HcalFarForwardZDCRawHits" "LFHCALRawHits" "LumiDirectPCALRawHits" "LumiSpecTrackerRawHits" "MPGDBarrelRawHits" "OuterMPGDBarrelRawHits" "PFRICHRawHits" "SiBarrelRawHits" "TaggerTrackerRawHits" "TOFBarrelRawHit" "TOFEndcapRawHits" "SiEndcapTrackerRawHits" "SiBarrelVertexRawHits")

nRecHitsArray=("B0ECalRecHits" "B0TrackerRecHits" "BackwardMPGDEndcapRecHits" "DIRCBarRecHits" "DRICHRecHits" "EcalBarrelImagingRecHits" "EcalBarrelScFiRecHits" "EcalEndcapNRecHits" "EcalEndcapPRecHits" "EcalEndcapPInsertRecHits" "EcalFarForwardZDCRecHits" "EcalLumiSpecRecHits" "ForwardMPGDEndcapRecHits" "ForwardOffMTrackerRecHits" "ForwardRomanPotRecHits" "HcalBarrelRecHits" "HcalEndcapNRecHits" "HcalEndcapPInsertRecHits" "HcalFarForwardZDCRecHits" "LFHCALRecHits" "LumiDirectPCALRecHits" "LumiSpecTrackerRecHits" "MPGDBarrelRecHits" "OuterMPGDBarrelRecHits" "PFRICHRecHits" "SiBarrelTrackerRecHits" "TaggerTrackerRecHits" "TOFBarrelRecHits" "TOFEndcapRecHits" "SiEndcapTrackerRecHits" "SiBarrelVertexRecHits")

# loop over all jobs
for jobID in $(seq 1 1 ${nJobs})
do
	# submit a job
	echo -ne "time eicrecon -Ppodio:output_include_collections=\"" >> ${runFile}
	echo -ne "MCParticles,LumiWindowCALHits" >> ${runFile}
	#- Sim Hits
	for str in ${nHitsArray[@]}
	do
		echo -ne ",${str}" >> ${runFile} 
	done
	#- Raw (Digi) Hits
	for str in ${nRawHitsArray[@]}
	do
		echo -ne ",${str}" >> ${runFile} 
	done
	#- Rec Hits
	for str in ${nRecHitsArray[@]}
	do
		echo -ne ",${str}" >> ${runFile} 
	done

	echo -ne "\" " >> ${runFile} 
	echo -ne "-Ppodio:output_file=${outputDirName}/output_eicrecon_${jobID}.podio.root " >> ${runFile} 
	echo "-Phistsfile=${outputDirName}/output_eicrecon_${jobID}.hist.root ${inputDirName}/output_eicsim_${jobID}.edm4hep.root" >> ${runFile}
	# -Pjana:nevents=1000 <-- to limit the number of events

	jobID_tmp=$(($jobID % 10))
	if [ $jobID_tmp -eq 0 ]; then
		# make the file executable
		chmod 777 ${runFile}
		
		# submit a job
		echo -ne "Run = ${runID} (last job = ${jobID}/${nJobs}): "
		bsub -q $queue -o ${outputDirName}/logJob_eicrecon_${jobID}.txt ". ${eicshell} -- ./${runFile}"

		# move to another run
		runID=$(($runID+1))
		runFile="runEICrecon_${runID}.sh"
		echo "#!/bin/bash" > ${runFile}
		echo "source ${eicsetup}" >> ${runFile}
		echo "export EICrecon_MY=${PWD}/../EICrecon_MY" >> ${runFile}
	fi
done

cd -


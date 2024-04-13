#!/bin/bash

jobID=$1
inputDirName=$2
outputDirName=$3
eicshell=$4
eicsetup=$5

runFile="runEICrecon_${jobID}.sh"
echo "#!/bin/bash" > ${runFile}
echo "source ${eicsetup}" >> ${runFile} 

nHitsArray=("B0ECalHits" "B0TrackerHits" "BackwardMPGDEndcapHits" "DIRCBarHits" "DRICHHits" "EcalBarrelImagingHits" "EcalBarrelScFiHits" "EcalEndcapNHits" "EcalEndcapPHits" "EcalEndcapPInsertHits" "EcalFarForwardZDCHits" "EcalLumiSpecHits" "ForwardMPGDEndcapHits" "ForwardOffMTrackerHits" "ForwardRomanPotHits" "HcalBarrelHits" "HcalEndcapNHits" "HcalEndcapPInsertHits" "HcalFarForwardZDCHits" "LFHCALHits" "LumiDirectPCALHits" "LumiSpecTrackerHits" "MPGDBarrelHits" "OuterMPGDBarrelHits" "PFRICHHits" "SiBarrelHits" "TaggerTrackerHits" "TOFBarrelHits" "TOFEndcapHits" "TrackerEndcapHits" "VertexBarrelHits")

nRawHitsArray=("B0ECalRawHits" "B0TrackerRawHits" "BackwardMPGDEndcapRawHits" "DIRCRawHits" "DRICHRawHits" "EcalBarrelImagingRawHits" "EcalBarrelScFiRawHits" "EcalEndcapNRawHits" "EcalEndcapPRawHits" "EcalEndcapPInsertRawHits" "EcalFarForwardZDCRawHits" "EcalLumiSpecRawHits" "ForwardMPGDEndcapRawHits" "ForwardOffMTrackerRawHits" "ForwardRomanPotRawHits" "HcalBarrelRawHits" "HcalEndcapNRawHits" "HcalEndcapPInsertRawHits" "HcalFarForwardZDCRawHits" "LFHCALRawHits" "LumiDirectPCALRawHits" "LumiSpecTrackerRawHits" "MPGDBarrelRawHits" "OuterMPGDBarrelRawHits" "PFRICHRawHits" "SiBarrelRawHits" "TaggerTrackerRawHits" "TOFBarrelRawHit" "TOFEndcapRawHits" "SiEndcapTrackerRawHits" "SiBarrelVertexRawHits")

nRecHitsArray=("B0ECalRecHits" "B0TrackerRecHits" "BackwardMPGDEndcapRecHits" "DIRCBarRecHits" "DRICHRecHits" "EcalBarrelImagingRecHits" "EcalBarrelScFiRecHits" "EcalEndcapNRecHits" "EcalEndcapPRecHits" "EcalEndcapPInsertRecHits" "EcalFarForwardZDCRecHits" "EcalLumiSpecRecHits" "ForwardMPGDEndcapRecHits" "ForwardOffMTrackerRecHits" "ForwardRomanPotRecHits" "HcalBarrelRecHits" "HcalEndcapNRecHits" "HcalEndcapPInsertRecHits" "HcalFarForwardZDCRecHits" "LFHCALRecHits" "LumiDirectPCALRecHits" "LumiSpecTrackerRecHits" "MPGDBarrelRecHits" "OuterMPGDBarrelRecHits" "PFRICHRecHits" "SiBarrelTrackerRecHits" "TaggerTrackerRecHits" "TOFBarrelRecHits" "TOFEndcapRecHits" "SiEndcapTrackerRecHits" "SiBarrelVertexRecHits")

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

chmod 777 ${runFile}

. ${eicshell} -- ./${runFile}

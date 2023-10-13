#!/bin/bash

threadID=$1;

echo "Bash version ${BASH_VERSION}..." > log_${threadID}.txt;

# GEANT4 env. variables
cd ~/geant4.10/geant4-build/; source geant4make.sh; cd -;

# ROOT env. variables
source ~/root/obj/bin/thisroot.sh;

chmod u+x exe;
chmod a+x exe;

runID=1;
incr=1;
nParticles=100000;

# LER
#mom=4.000000000; # [GeV/c]
#parType="e+";

# HER
mom=7.007290000; # [GeV/c]
parType="e-";

#for length in $(seq 0.0 1.0 50.0)  # for W
#for length in $(seq 0.0 1.0 50.0)  # for Ta
#for length in $(seq 0.0 4.0 200.0) # for Cu
#for length in $(seq 0.0 4.0 120.0)  # for GRAPHITE
#for length in $(seq 0.0 5.0 240.0)  # for GRAPHITE
#for length in $(seq 0.0 2.0 100.0)  # for Ti
#for length in $(seq 0.0 2.0 100.0)  # for MoGr

for length in $(seq 0.0 2.0 100.0)  # for MoGr
do
#    for deltaP in $(seq -0.02 0.001 0.02)
#    do
        deltaP=0;
        echo "##-----------------------------------------##"                >  ./macFiles/run_${threadID}_${runID}.mac;
        echo "/run/verbose 1"                                               >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/event/verbose 0"                                             >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/tracking/verbose 0"                                          >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "##-----------------------------------------##"                >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/target/length ${length} mm"                                  >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/target/width 500.0 mm"                                       >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/target/height 500.0 mm"                                      >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/output/filename ../output/output_${threadID}_${runID}.root"  >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/stack/killsecondaries 1"                                     >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/beam/deltaP ${deltaP}"                                       >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/beam/P ${mom} GeV"                                           >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/beam/pName ${parType}"                                       >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/beam/PdirX 0.0"                                              >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/beam/PdirY 0.0"                                              >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/beam/PdirZ 1.0"                                              >> ./macFiles/run_${threadID}_${runID}.mac;
        echo ""                                                             >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/run/initialize"                                              >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "##-----------------------------------------##"                >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "/run/beamOn ${nParticles}"                                    >> ./macFiles/run_${threadID}_${runID}.mac;
        echo "##-----------------------------------------##"                >> ./macFiles/run_${threadID}_${runID}.mac;

        cat ./macFiles/run_${threadID}_${runID}.mac >> log_${threadID}.txt;
        ./exe ./macFiles/run_${threadID}_${runID}.mac >> log_${threadID}.txt;
        runID=$(($runID+$incr));
#    done
done


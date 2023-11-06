#!/bin/bash

# load geometry
source /opt/detector/setup.sh

# load EICrecon env vars
source /home/belle2/anatochi/eic/EICrecon/bin/eicrecon-this.sh

rm -rf ${PWD}/EICrecon_MY
mkdir -pv ${PWD}/EICrecon_MY
export EICrecon_MY=${PWD}/EICrecon_MY

rm -rf analysisPlugin/build
cmake -S analysisPlugin -B analysisPlugin/build
cmake --build analysisPlugin/build --target install

#o
rm -rf build
mkdir -pv build
cd build
#eicrecon -Pjana:nevents=1000 -Pplugins=analysisPlugin ../SR_out_int_window_100.0ns_nevents_40000_pid_22_status_4_1_seed_1.edm4hep.root
eicrecon -Pplugins=analysisPlugin ../../eic_sim/output/output.edm4hep.root
cd -


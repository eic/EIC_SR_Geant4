#!/bin/bash

generator_repository_release_tag='dataprod_rel_1.0.0'
physics_processes='synrad'
electron_momentum='18'
proton_momentum='275'

main_dir='/gpfs02/eic/anatochii/SynradG4_HepMC_Files_SR_on_IP6'
input_hepmc3_dir="${main_dir}/SynradG4_v6.3.1_20240906_18x275GeV_HepMCfiles_IPphotons_HepMC3/${physics_processes}/${generator_repository_release_tag}/${electron_momentum}x${proton_momentum}"
output_hepmc3_dir="${main_dir}/SynradG4_v6.3.1_20240906_18x275GeV_HepMCfiles_IPphotons_HepMC3_ROOT/${physics_processes}/${generator_repository_release_tag}/${electron_momentum}x${proton_momentum}"
mkdir -pv ${output_hepmc3_dir}
nRuns=1

date

mkdir -pv build
cd build
cmake ../
make -j2

for runID in $(seq 1 1 ${nRuns})
do
	file_name="${generator_repository_release_tag}_${physics_processes}_${electron_momentum}x${proton_momentum}_run${runID}"

	./hepmc3ascii2root ${input_hepmc3_dir}/${file_name}.hepmc3 ${output_hepmc3_dir}/${file_name}.hepmc3.tree.root &> log_${runID}.txt
	echo "${runID} : ${output_hepmc3_dir}/${file_name}.hepmc3.tree.root"
done


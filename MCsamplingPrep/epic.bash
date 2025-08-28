#!/bin/bash
outputDir='/gpfs02/eic/anatochii/MCsamplingPrep_2'
fileName='dataprod_rel_1.0.0_synrad_18x275_run001'

eicshell='/gpfs02/eic/anatochii/eic/eic-shell'
eicconfig='/gpfs02/eic/anatochii/eic/epic/install/bin/thisepic.sh'

${eicshell} << EOF
eicconfig='/gpfs02/eic/anatochii/eic/epic/install/bin/thisepic.sh'
source ${eicconfig}
time npsim --runType batch --numberOfEvents -1 --compactFile \${DETECTOR_PATH}/\${DETECTOR_CONFIG}.xml --outputFile ${outputDir}/${fileName}.preproc.hepmc3.tree.edm4hep.root --physics.list FTFP_BERT --inputFiles ${outputDir}/${fileName}.preproc.hepmc3.tree.root
time eicrecon -Ppodio:output_collections=SiBarrelHits,TrackerEndcapHits,VertexBarrelHits,SiBarrelRawHits,SiEndcapTrackerRawHits,SiBarrelVertexRawHits,SiBarrelTrackerRecHits,SiEndcapTrackerRecHits,SiBarrelVertexRecHits -Ppodio:output_file=${outputDir}/${fileName}.preproc.hepmc3.tree.podio.root ${outputDir}/${fileName}.preproc.hepmc3.tree.edm4hep.root
date
EOF

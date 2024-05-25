# EIC_SR_Geant4
Geant4 stand-alone simulation of synchrotron radiation in the EIC interaction region (IR6)


# pre-requirements

[Geant4](https://root.cern)
[ROOT6](https://root.cern)
[BOOST](https://www.boost.org)
[HEPMC3](http://hepmc.web.cern.ch/hepmc/)



#################################
#				#
# USEFULL INSTRUCTIONS/COMMANDS #
#				#
#################################
# Install eic-shell
# https://eic.phy.anl.gov/tutorials/eic_tutorial/getting-started/quickstart/
#
#-- To build EIC
# git clone https://github.com/eic/epic
# (or git clone git@github.com:eic/epic)
# cd epic
# cmake -S . -B build
# cmake --build build --target install -- -j1
#
#-- To load det configs and env vars
# source /opt/detector/setup.sh 
#
#-- To convert det geometry from XML to ROOT
# dd_web_display --export $DETECTOR_PATH/$DETECTOR_CONFIG.xml
#
#-- To run simulation with a particle gun
# ddsim --steeringFile doubleElectron_negativeDir.py --compactFile $DETECTOR_PATH/$DETECTOR_CONFIG.xml -G -N 10 --outputFile ee_1GeV_10GeV_EndcalN_1e1.edm4hep.root --part.minimalKineticEnergy "1*TeV"
#
#-- To get HEPMC files from S3/SDCC
#
#- Load the server
# mc config host add S3 https://dtn01.sdcc.bnl.gov:9000 eicS3read eicS3read
#
#- Find the file
# mc ls S3/eictest/EPIC/Tutorials/
#
#- Take only first 20k lines
# mc head -n 20000 S3/eictest/EPIC/Tutorials/pythia8NCDIS_10x100_minQ2=1_beamEffects_xAngle=-0.025_hiDiv.hepmc > pythia8NCDIS_10x100.hepmc
#
#- To run simulation with HEPMC events
# ddsim --steeringFile --compactFile $DETECTOR_PATH/$DETECTOR_CONFIG.xml -numberOfEvents 10 --outputFile pythia8NCDIS_10x100.edm4hep.root
#
#- To run simulation with HEPMC events + optical photon physics
# npsim --steeringFile --compactFile $DETECTOR_PATH/$DETECTOR_CONFIG.xml -numberOfEvents 10 --outputFile npsim_pythia8NCDIS_10x100.edm4hep.root
#
# EICrecon is now automatically loaded with eic-shell -- no needs to install and load it separately
#-- To build EICreco
# in eic-shell
# git clone https://github.com/eic/EICrecon
# (or git clone git@github.com:eic/EICrecon)
# cd EICrecon
# cmake -S . -B build
# cmake --build build --target install -- -j1
#
#-- To load EICrecon env vars
# source EICrecon/bin/eicrecon-this.sh
#
#-- To run EIC reconstruction on the simulated data file with only defined branches
# eicrecon -Ppodio:output_include_collections=ReconstructedParticles,MCParticles,EcalEndcapNRawHits pythia8NCDIS_10x100.edm4hep.root




# Copy HEPMC3 files from gpfs:
# for runID in {1..50}; do sftp anatochii@sftp.sdcc.bnl.gov:/gpfs02/eic/bpage/home/EPIC/officialSimu/pythia6/sidis_minbias/HEPMCFILES/pythia_ep_noradcor_18x275_q2_0.000000001_1.0_run${runID}.ab.hepmc.gz

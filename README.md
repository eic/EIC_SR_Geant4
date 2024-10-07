Here’s a README for the EIC_SR_Geant4 repository based on the general structure and purpose of such a project:

---

# EIC_SR_Geant4

**EIC_SR_Geant4** is a Geant4-based simulation framework for modeling synchrotron radiation (SR) in the Electron-Ion Collider (EIC). This tool aims to simulate the generation, propagation, and interaction of synchrotron radiation photons with the vacuum chamber, taking into account effects like surface roughness and diffuse/specular reflection.

## Features
- **Synchrotron Radiation Simulation**: Models synchrotron radiation photon generation using realistic magnetic field configurations.
- **Photon Propagation and Interaction**: Simulates photon interactions with the vacuum chamber surface, including:
  - **Specular and Diffuse Reflection**: Incorporates models to account for surface roughness and the probabilistic nature of photon reflection.
  - **Material Effects**: Handles different vacuum chamber materials and coatings to study the impact on photon behavior.
- **Customizable Geometry**: Supports user-defined geometries for vacuum chambers, enabling detailed modeling for different beam pipe designs.

## Requirements

### Software Dependencies
- **Geant4** (version 11.2.0 or later)
- **CMake** (version 3.12 or later)
- **GCC** (version 9.3 or later recommended)
- Additional libraries: **CLHEP**, **ROOT** (optional but recommended for output visualization)

### System Requirements
- Linux-based system (e.g., CentOS, Ubuntu)
- At least 4 GB of RAM and 2 GB of disk space for compiling and running simulations.

## Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/eic/EIC_SR_Geant4.git
   cd EIC_SR_Geant4
   ```

2. **Set up Geant4**:
   Make sure Geant4 is installed and sourced correctly. Refer to the [Geant4 installation guide](https://geant4.web.cern.ch/support/download) for instructions.

3. **Build the project**:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. **Run the executable**:
   ```bash
   ./EIC_SR_Geant4
   ```

## Usage

The executable supports various input parameters to customize the simulation. You can provide a configuration file or pass arguments directly to the command line. 

### Example Command
```bash
./EIC_SR_Geant4 -config config/simulation_config.in
```

### Configuration File

The configuration file allows specifying:
- Magnetic field parameters
- Vacuum chamber geometry
- Surface properties (e.g., roughness, material type)
- Output settings (e.g., file format, histogram options)

Refer to the `config/example_config.in` file for a template.

## Output

The simulation outputs various data files, including:
- **Photon spectra**: Energy and angular distributions of SR photons.
- **Reflection statistics**: Details of photon interactions with surfaces, including specular and diffuse components.
- **ROOT histograms** (if ROOT is enabled): For visual analysis of SR characteristics.

## Contributing

Contributions are welcome! Please follow these steps:
1. Fork the repository.
2. Create a new branch for your feature/bugfix.
3. Submit a pull request with a detailed description of the changes.

## Issues

If you encounter any issues or bugs, please open an issue on the GitHub repository with a clear description and, if possible, steps to reproduce.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.

## Acknowledgments

Special thanks to the Electron-Ion Collider Collaboration and the Geant4 development team for their support and resources.










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

# FreeCAD: Charlie's STL file: coordinate system -> EPIC coord. system: open STL mode, go to Edit->Transform, and rotate around Y-axis by 180 deg; use https://imagetostl.com/convert/file/stl/to/obj#google_vignette for STL to OBJ

# Count the number of files:
# for beamEnergy in '18x275' '10x275' '5x100'; do echo "--> ${beamEnergy}"; for mask in 'nosrmask' 'srmask'; do echo ">> ${mask}"; ls /gpfs02/eic/anatochii/output_eicrecon_v6.3.1_${mask}_${beamEnergy}GeV_highStat/logExe_*.txt | wc -l; done; done

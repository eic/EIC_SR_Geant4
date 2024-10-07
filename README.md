# Electron-Ion Collider Synchrotron Radiation Simulation in Geant4 (EIC_SR_Geant4_

**EIC_SR_Geant4** is a Geant4-based simulation framework for modeling synchrotron radiation (SR) in the Electron-Ion Collider (EIC). This tool aims to simulate the generation, propagation, and interaction of SR photons with the vacuum chamber, taking into account effects like surface roughness and diffuse/specular reflection. For a comprehensive overview of the physics processes implemented in the code, please refer to the following paper: [https://doi.org/10.48550/arXiv.2408.11709](https://doi.org/10.48550/arXiv.2408.11709).

## Features
- **SR Simulation**: Models SR photon generation using realistic magnetic field configurations.
- **Photon Propagation and Interaction**: Simulates photon interactions with the vacuum chamber surface, including:
  - **Specular and Diffuse Reflection**: Incorporates models to account for surface roughness and the probabilistic nature of photon reflection.
  - **Material Effects**: Handles different vacuum chamber materials and coatings to study the impact on photon behavior.
- **Customizable Geometry**: Supports user-defined geometries for vacuum chambers, enabling detailed modeling for different beam pipe designs, including detailed 3D model improting as an .OBJ file.

## Requirements

### Software Dependencies
- **[Geant4](https://geant4.org)** (version 11.2.0 or later)
- **[CMake](https://cmake.org)** (version 3.12 or later)
- **[GCC](https://gcc.gnu.org)** (version 9.3 or later recommended)
- **[ROOT](https://root.cern)** (version 6.30 or later)
- **[HepMC3](https://gitlab.cern.ch/hepmc/HepMC3)**
**NOTE**: "The [eic-shell](https://github.com/eic/eic-shell) environment is fully equipped with all the necessary software packages and dependencies required to compile, execute, and manage the project efficiently, ensuring a streamlined and consistent development experience."

### System Requirements
- Linux-based system (e.g., CentOS, Ubuntu, MacOS)
- At least 4 GB of RAM and 2 GB of disk space for compiling and running simple simulations.

## Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/eic/EIC_SR_Geant4.git
   cd EIC_SR_Geant4
   ```

2. **Set up Software**:
   Make sure Geant4, ROOT, and HepMC3 are installed and sourced correctly. Refer to the [Geant4 installation guide](https://geant4.web.cern.ch/support/download), [ROOT installation guide](https://root.cern/install/), and [HepMC3 installation guide](https://gitlab.cern.ch/hepmc/HepMC3/-/blob/master/README.md?ref_type=heads) for instructions, respectively.

## Usage
1. **SR sample production**:
   - Go to the main Geant4 directory:
     ```bash
     cd geant4_sim
     ```
     
   - Build the project:
     ```bash
     mkdir build
     cd build
     cmake ../
     make
     ```
     
   - Run the executable:
     The compilation process generates an executable file called `exe`. By running `./exe`, an error message will be displayed, providing instructions on how to properly run the script with the correct input parameters and format.
      ```bash
      ./exe [mac_file] [xml_file] [random_number]
      ```
      Here, `mac_file` is a a macro script that contains a set of pre-defined commands including the output file name and simulation statistics, written in the Geant4 macro language, `xml_file` is used to define and manage various aspects of the simulation configuration, such as geometry, detector setup, materials, and parameters, the XML file allows to structure and organize the data in a clear, hierarchical format that can be easily read by both humans and machines, and `random_number` is a random integer number used to generate a random number for the Geant4 random generator engine (`G4Random::setTheSeed(randomSeed)`) as `randomSeed = current_time_in_seconds + random_number` (`current_time_in_seconds` is given by the internal function in the code). Please use `run.mac` or `vis.mac` for large sample data production or simulation geometry visualization only, respectively.
   
   - To simplify the simulation process on high-performance computing resources like **KEKCC** at KEK, **SDCC** at BNL, **iFarm** at JLab, or **NERSC** at LBNL, a dedicated set of bash scripts is provided. These scripts—`runJobs_kekcc.bash`, `runJobs_sdcc.bash`, `runJobs_jlab.bash`, and `runJobs_nersc.bash`—are tailored for each respective server. The scripts handle job submission for high-statistics data production. To use them, simply run the appropriate script from the `geant4_sim` directory:
     ```bash
     ./runJobs_[server_name].bash
     ```
     where `server_name` could be `kekcc`, `sdcc`, `jlab`, or `nersc`.

### A few comments about the input XML file:

1. The file is divided into sections, each describing a part of the simulation, such as geometry, magnetic field, X-ray reflection mode, etc.

2. Under the `srmodel` section, you can select the SR production physics model from standard Geant4 libraries, such as `G4SynchrotronRadiation` or `G4SynchrotronRadiationInMat`. The X-ray reflection model is defined by the parameter `type`. For a comprehensive reflection model, including specular and diffuse reflection similar to Synrad+, use `type=3`.

3. In the same section, you can define the roughness of the beam pipe surface. Currently, the same roughness is applied to all materials in the simulation.

4. To store X-ray tracks from the production vertex in a magnet to the absorption position, use the `storetrj` section.

5. To improve computational efficiency, the `gammacut` section defines energy and coordinate cuts for X-ray photons.

6. The `meanfreepath` section allows you to adjust the SR production cross-section. By changing the `MeanFreePathFactor` parameter from 1 to 10, you can increase the number of SR photons produced in a magnet by a factor of 10, improving CPU time usage.

7. The `tcut` parameter in the `timecut` section limits how long particles are tracked.

8. To stop particle tracking at a specific boundary, use the `bwdgeocut` section.

9. The electron beam distribution is defined in the `generator` section. Beam parameters are sourced from the ESR lattice file provided by the EIC optics group, located in `geometry/lattice_vXX` (where `XX` represents the optics version, e.g., `6.3` or `6.3.1`). For lattice file version control, refer to the [EIC SharePoint](https://brookhavenlab.sharepoint.com/sites/eRHIC/bnl&slac).

10. Since the modeled geometry is limited, the `absorbers` section describes absorbers used to prevent particles from propagating beyond the defined beam pipe vacuum geometry.

11. All magnets, including the detector solenoid, are described in the `magnets` section.

12. The simulated beam pipe, provided by the EIC vacuum group, is imported as an `.OBJ` file and converted into a tessellated volume using the [CADMesh class](https://github.com/christopherpoole/CADMesh). The path to the XML file is specified in the `beampipe` section. **Note**: Geant4’s OGL visualizer may not correctly render the 3D model due to the complexity of the beam pipe geometry, but this does not affect the simulation results, only the visualization.

13. The EIC_SR_Geant4 framework can use the input HepMC3 files to generate, for example, low-Q^2 electrons at the interaction point (IP) at (0;0;0) cm. To produce the input HepMC files, one can use one of the approaches and then select the corresponding option in the XML file section `genname`: `genname=Beam` - generates beam particles, `genname=HepMC` - uses Pythia HepMC files, `genname=HepMC_GETaLM` - uses GETaLM files (Bremsstrahlung or Quasi-Real):
    - `Beam`: based on the lattice file; see `geometry/printLatticeParameters.C`;
    - `HepMC`: electrons are taken from the Pythia file stored on SDCC: `/gpfs02/eic/bpage/home/EPIC/officialSimu/pythia6/sidis_minbias/HEPMCFILES/`
    - `HepMC_GETaLM`: electrons are produced using the [GETaLM generator](https://doi.org/10.1016/j.cpc.2021.108251).

## Output

The simulation outputs ROOT data files, including:
- **Photon spectra**: 

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

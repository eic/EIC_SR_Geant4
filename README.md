# Electron-Ion Collider Synchrotron Radiation Simulation in Geant4 (EIC_SR_Geant4)

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
### I. Geant4 step: SR sample production ###
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
   
   - To simplify the simulation process on high-performance computing resources like **KEKCC** at KEK, **SDCC** at BNL, **iFarm** at JLab, or **NERSC** at LBNL, a dedicated set of bash scripts is provided. These scripts—`runJobs_kekcc.bash`, `runJobs_sdcc.bash`, `runJobs_jlab.bash`, and `runJobs_nersc.bash` — are tailored for each respective server. The scripts handle job submission for high-statistics data production. To use them, simply run the appropriate script from the `geant4_sim` directory:
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
    
    13. The EIC_SR_Geant4 framework can use the input HepMC files to generate, for example, low-Q^2 electrons at the interaction point (IP) at (0;0;0) cm. To produce the input HepMC files, one can use one of the approaches and then select the corresponding option in the XML file section `genname`: `genname=Beam` - generates beam particles, `genname=HepMC` - uses Pythia HepMC files, `genname=HepMC_GETaLM` - uses GETaLM files (Bremsstrahlung or Quasi-Real):
        - `Beam`: based on the lattice file; see `geometry/printLatticeParameters.C`;
        - `HepMC`: electrons are taken from the Pythia file stored on SDCC: `/gpfs02/eic/bpage/home/EPIC/officialSimu/pythia6/sidis_minbias/HEPMCFILES/`
        - `HepMC_GETaLM`: electrons are produced using the [GETaLM generator](https://doi.org/10.1016/j.cpc.2021.108251).

- **Output**: The simulation outputs ROOT data files that contain the following three ROOT trees:
  - **xml**: Contains the simulation settings, including the XML file.
  - **tracks**: Contains information about SR photon tracks that reached the beam pipe region defined in the XML file.
  - **tree**: Full simulation information, including generated particle distributions, absorbed SR photons, low-Q² sensors, luminosity monitors, and production vertices of SR photons. This tree is used for further analysis.
  
### II. Analysis step: Prepare ROOT and HepMC files ###

- In `geant4_ana/`, run:
  ```bash
  mkdir build
  cd build
  cmake ../
  make
  ```
- Run the executable:
    - The compilation process generates an executable file called `exe`. By running `./exe`, an error message will be displayed, providing instructions on how to properly run the script with the correct input parameters and format.
      ```bash
      ./exe [input_dir_name] [input_file_name] [output_dir_name] [output_file_name] [input_tree_name] [mix_Z] [max_Z] [beampipe_radius]
      ```
      Here, `input_dir_name`, `input_file_name`, and `input_tree_name` are the input directory, file, and tree names. `output_dir_name` and `output_file_name` are the output directory and file names. `mix_Z` and `max_Z` defines the region where the absorbed SR photons are collected and stored in the output file name. `beampipe_radius` is not used currently, default is `-1`.
    - To simplify the analysis process on high-performance computing resources like **KEKCC** at KEK, **SDCC** at BNL, or **iFarm** at JLab, a dedicated set of bash scripts is provided. These scripts—`runAnaJobs_kekcc.bash`, `runAnaJobs_sdcc.bash`, and `runAnaJobs_jlab.bash` — are tailored for each respective server.
      
- **Output**: The analysis outputs ROOT files with histograms and HepMC files with the absorbed SR photons within the define region of the beam pipe for further ePIC detector response study through the DD4Hep simulation in eic-shell.
- **Merging**: In the case of hundreds of HepMC files, it is recommended to merge them into a smaller number of HepMC files:
  ```bash
  cd geant4_ana/merge_hepmc
  mkdir build
  cd build
  cmake ../
  make
  ./exe [output_file_name] [input_file_name_pattern] [number_of_input_files]
  ```
  The code will create a single HepMC file with non-empty events of absorbed SR photons.
- **Final Analysis**: To analyse the single HepMC file (draw SR photon distributions):
  ```bash
  cd geant4_ana/merge_hepmc/ana
  mkdir build
  cd build
  cmake ../
  make
  ./exe [input_file_name]
  ```
  The code creates an output ROOT file containing a ROOT tree with the coordinates of absorbed SR photons.
  
### III. Auxiliarly step: Detector response and SR track study ###
- Use `eic_sim` to run the DD4Hep simulation.
- Use `eic_recon` to analyze the DD4Hep outputs.
- Use `plotTracks.C` to plot the stored SR trackes after the Geant4 simulation.


## Contributing

Contributions are welcome! Please follow these steps:
1. Fork the repository.
2. Create a new branch for your feature/bugfix.
3. Submit a pull request with a detailed description of the changes.

## Issues

If you encounter any issues or bugs, please open an issue on the GitHub repository with a clear description and, if possible, steps to reproduce.

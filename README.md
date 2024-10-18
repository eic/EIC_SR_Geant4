# Electron-Ion Collider Synchrotron Radiation Simulation in Geant4 (EIC_SR_Geant4)

## Disclaimer

> **This software is provided "as-is" without any express or implied warranties, including, but not limited to, the implied warranties of merchantability and fitness for a particular purpose. In no event shall the authors or contributors be held liable for any damages arising from the use of this software.**
>
> **Use at your own risk. Ensure you have proper backups and validation procedures in place when using this software in a production environment.**
>
> **Steering scripts for the ePIC/EIC data production are labeled with the prefix `DataProd_` and were used on iFarm server at JLab.**


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
- **[EIC-SHELL](https://github.com/eic/eic-shell)**

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
   Make sure Geant4, ROOT, HepMC3, and EIC-SHELL are installed and sourced correctly. Refer to the [Geant4 installation guide](https://geant4.web.cern.ch/support/download), [ROOT installation guide](https://root.cern/install/), [HepMC3 installation guide](https://gitlab.cern.ch/hepmc/HepMC3/-/blob/master/README.md?ref_type=heads), and [EIC-SHELL installation guide](https://github.com/eic/eic-shell) for instructions, respectively.

## SR data production on **JLab iFarm**
### I. Geant4 step: SR sample production ###
   - Go to the main Geant4 directory:
     ```bash
     cd geant4_sim
     ```
     
   - Run:
     ```bash
     ./DataProd_runJobs_jlab.bash [global path for output files] [eic-shell]
     ```
- **Output**: The simulation outputs ROOT data files that contain the following three ROOT trees:
  - **xml**: Contains the simulation settings, including the XML file.
  - **tracks**: Contains information about SR photon tracks that reached the beam pipe region defined in the XML file.
  - **tree**: Full simulation information, including generated particle distributions, absorbed SR photons, low-Q² sensors, luminosity monitors, and production vertices of SR photons. This tree is used for further analysis.
  
### II. Analysis step: Prepare ROOT and HepMC files ###

   - In `geant4_ana/`, run:
     ```bash
     ./DataProd_runAnaJobs_jlab.bash [global path for input/output files] [eic-shell]
     ```
- **Output**: The analysis outputs ROOT files with histograms and HepMC files with the absorbed SR photons within the define region of the beam pipe for further ePIC detector response study through the DD4Hep simulation in eic-shell.
- **Merging**: In the case of hundreds of HepMC files, it is recommended to merge them into a smaller number of HepMC files:
   - Go to `geant4_ana/merge_hepmc` and run:
     ```bash
     ./DataProd_runMerge_jlab.bash [global path for output files] [eic-shell] 
     ```
  The code will create a single HepMC file with non-empty events of absorbed SR photons.
   - To convert the output HepMC files to HepMC3.ROOT, please use [hepmc3ascii2root](https://github.com/eic/hepmc3ascii2root).

## Issues

If you encounter any issues or bugs, please open an issue on the GitHub repository with a clear description and, if possible, steps to reproduce.

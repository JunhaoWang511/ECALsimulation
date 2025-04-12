# Geant4 simulation for ECAL

## What is Geant4

Geant4 is a toolkit for the simulation of the passage of particles through matter. Its areas of application include high energy, nuclear and accelerator physics, as well as studies in medical and space science.

## What is ECAL

The ECAL (electromagnetic calorimeter) sub-detector of [**STCF**](https://stcf.ustc.edu.cn/ "STCF project website") is a cylindrical array of scintillating crystals that provides energy and position measurements for photons with high resolution.


## Required dependencies

- [Geant4 10.5](https://geant4.web.cern.ch/ "Geant4 official website")
- [ROOT 6.26](https://root.cern/ "ROOT official website")
- CMAKE 3.16 or higher
- C++ standard 17 or higher
- Qt 5.15 or higher
- GSL (GNU Scientific Library)

## Introduction
This simulation is inherited from zkjia's codes.
### Geometry description

The main volume is a pure CsI crystal with NOL coating for wavelength shifting and wrapped with teflon outside for reflection, besides, one endcap is coupled with an APD for photon detection. 

### Physics list

The basic physics list is **QGSP_BERT** with EM model replaced by **G4EmStandardPhysics_option4**, and **G4OpticalPhysics** is controlled to be on/off.

### Optical parameters

All parameters are digitized from plots in literatures, including CsI's scintillator emission/absorption and light yield, NOL's emission/absorption, APD's quantum efficiency etc.

However, some parameters are still undetermined, including CsI's absorption and scattering length, NOL's refractive index and absorption length.

### Primary particle

**G4GeneralParticleSource** class is chosen as particle generator, and particles like electron('e-'), gamma('gamma'), muon('mu-'), pion('pi-'), proton('proton') ranging from 10 MeV to 3 GeV are used in our simulation.

### Data structure

The simulation result is stored event by event in a root file. For each event, we record some physical amounts including energy deposition in crystal, photon's generation, absorption and detection, what's more, we also record energy, time and position for every single detected optical photons.

## How to run

1. Download or clone source code in Linux system. (suppose absolute path is */home/username/EcalSimulation*)
2. Switch working directory. (`cd /home/username/EcalSimulation`)
3. Make build directory and enter in. (`mkdir build` && `cd ./build`)
4. Cmake and generate executable. (`cmake ..` && `make`)
5. Run the executable (`./ECAL (-i) (-e xxx.mac) (-o xxx.root) (-n x) (-op) (-h)`) with some arguments linked behind:\
-i  &emsp;        Run in interactive mode\
-e  &emsp;        Execute specified marco file \
-o  &emsp;        Specify the name of output file (default: ECAL.root )\
-n  &emsp;        Number of threads to use (default: 8 )\
-op &ensp;        Use PhysicsList with optics\
-h  &emsp; Print this message and exit 
1. The simulation result is stored in a root file named `ECAL.root` by default.
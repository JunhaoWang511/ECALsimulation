#ifndef HistoManager_h
#define HistoManager_h 1

#include "globals.hh"
#include <vector>
#include "g4root.hh"
#include "G4ThreeVector.hh"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

class ParticleInfo
{
public:
  G4int fEventID;
  G4int fRunID;
  TString fParticle;
  G4double fPrimaryEnergy;
  G4int fPhotonGen;
  G4int fPhotonSelfAbs;
  G4int fPhotonBounAbs;
  G4int fPhotonDet;
  G4int fPhotonWLS;
  G4double fEnergyDeposition;
  std::vector<G4double> fPhotonEnergy;
  std::vector<G4double> fPhotonGlobalTime;
  std::vector<G4double> fPhotonLocalTime;
  std::vector<G4double> fPhotonPositionX;
  std::vector<G4double> fPhotonPositionY;
  std::vector<G4double> fPhotonPositionZ;
  std::vector<G4double> fPhotonKinetic;
  G4String fDecayChain;
  G4int fPhotonGlobalTimeHis[5000];

  void reset()
  {
    fRunID = 0;
    fEventID = 0;
    fParticle = "";
    fPrimaryEnergy = 0;
    fPhotonGen = 0;
    fPhotonSelfAbs = 0;
    fPhotonBounAbs = 0;
    fPhotonDet = 0;
    fPhotonWLS = 0;
    fEnergyDeposition = 0;
    fPhotonEnergy.clear();
    fPhotonGlobalTime.clear();
    fPhotonLocalTime.clear();
    fPhotonPositionX.clear();
    fPhotonPositionY.clear();
    fPhotonPositionZ.clear();
    fPhotonKinetic.clear();
    fDecayChain = "";
    memset(fPhotonGlobalTimeHis, 0, sizeof(fPhotonGlobalTimeHis));
  }

  ParticleInfo()
  {
    fRunID = 0;
    fEventID = 0;
    fParticle = "";
    fPrimaryEnergy = 0;
    fPhotonGen = 0;
    fPhotonSelfAbs = 0;
    fPhotonBounAbs = 0;
    fPhotonDet = 0;
    fPhotonWLS = 0;
    fEnergyDeposition = 0;
    fPhotonEnergy.clear();
    fPhotonGlobalTime.clear();
    fPhotonLocalTime.clear();
    fPhotonPositionX.clear();
    fPhotonPositionY.clear();
    fPhotonPositionZ.clear();
    fDecayChain = "";
    fPhotonKinetic.clear();
    memset(fPhotonGlobalTimeHis, 0, sizeof(fPhotonGlobalTimeHis));
  }
  ~ParticleInfo() {};
};

class HistoManager
{
public:
  HistoManager(G4String fOutname = "ecal.root");
  ~HistoManager();
  static HistoManager *Instance();

public:
  void save();
  void book();

  void SetParticleInfo(ParticleInfo);

public:
  ParticleInfo fParticleInfo;
  TFile *fRootFile;
  TTree *fNtuple;

private:
  static HistoManager *fgInstance;
  G4String fOutName;
};

#endif

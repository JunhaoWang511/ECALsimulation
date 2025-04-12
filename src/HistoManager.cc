#include "HistoManager.hh"
#include "G4UnitsTable.hh"
#include "TTree.h"
#include "TFile.h"
#include <pthread.h>

pthread_mutex_t mutex_instance;
pthread_mutex_t mutex_fill;

HistoManager *HistoManager::fgInstance = 0;
HistoManager *HistoManager::Instance()
{
  if (fgInstance == NULL)
  {
    pthread_mutex_lock(&mutex_instance);
    if (fgInstance == NULL)
      fgInstance = new HistoManager();
    pthread_mutex_unlock(&mutex_instance);
  }
  return fgInstance;
}

HistoManager::HistoManager(G4String foname)
    : fRootFile(0), fNtuple(0), fOutName(foname)
{
  pthread_mutex_init(&mutex_instance, NULL);
  pthread_mutex_init(&mutex_fill, NULL);
  fgInstance = this;
  book();
}

HistoManager::~HistoManager()
{
  save();
  fgInstance = 0;
}

void HistoManager::book()
{
  G4cout << "---Create Root file--- " << G4endl;
  fRootFile = new TFile(fOutName.c_str(), "RECREATE");
  fNtuple = new TTree("ecal", "data of ecal");
  fNtuple->Branch("EventID", &fParticleInfo.fEventID);
  fNtuple->Branch("RunID", &fParticleInfo.fRunID);
  fNtuple->Branch("PhotonGeneration", &fParticleInfo.fPhotonGen);
  fNtuple->Branch("PhotonWLSConversion", &fParticleInfo.fPhotonWLS);
  fNtuple->Branch("PhotonSelfAbsorption", &fParticleInfo.fPhotonSelfAbs);
  fNtuple->Branch("PhotonBoundaryAbsorption", &fParticleInfo.fPhotonBounAbs);
  fNtuple->Branch("PhotonDetection", &fParticleInfo.fPhotonDet);
  fNtuple->Branch("EnergyDeposition", &fParticleInfo.fEnergyDeposition);
  fNtuple->Branch("PhotonEnergy", &fParticleInfo.fPhotonEnergy);
  fNtuple->Branch("PhotonGlobalTime", &fParticleInfo.fPhotonGlobalTime);
  fNtuple->Branch("PhotonLocalTime", &fParticleInfo.fPhotonLocalTime);
  fNtuple->Branch("PhotonPositionX", &fParticleInfo.fPhotonPositionX);
  fNtuple->Branch("PhotonPositionY", &fParticleInfo.fPhotonPositionY);
  fNtuple->Branch("PhotonPositionZ", &fParticleInfo.fPhotonPositionZ);
}

void HistoManager::save()
{
  fNtuple->Write();
  fRootFile->Close();
  G4cout << "---close rootfile---" << G4endl;
}

void HistoManager::SetParticleInfo(ParticleInfo parinfo)
{
  pthread_mutex_lock(&mutex_fill);
  fParticleInfo = parinfo;
  fNtuple->Fill();
  pthread_mutex_unlock(&mutex_fill);
}

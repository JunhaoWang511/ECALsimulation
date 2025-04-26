#include "EventAction.hh"

#include "G4Event.hh"
#include <iomanip>
#include "HistoManager.hh"
#include "TTree.h"
#include "RunAction.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "Run.hh"

EventAction::EventAction(RunAction *aRunAction)
    : G4UserEventAction(), fRunAction(aRunAction), fHistoManager()
{
  fHistoManager = HistoManager::Instance();
}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event *aEvent)
{
  fParticleInfo.reset();
  G4int fRunID = fRunAction->getRunID();
  fParticleInfo.fRunID = fRunID;
  fParticleInfo.fEventID = aEvent->GetEventID();
  G4PrimaryParticle *particle = aEvent->GetPrimaryVertex()->GetPrimary();
  fParticleInfo.fParticle = particle->GetParticleDefinition()->GetParticleName();
  fParticleInfo.fPrimaryEnergy = particle->GetKineticEnergy() / MeV;
  fParticleInfo.fPrimaryDirection[0] = particle->GetMomentumDirection().getX();
  fParticleInfo.fPrimaryDirection[1] = particle->GetMomentumDirection().getY();
  fParticleInfo.fPrimaryDirection[2] = particle->GetMomentumDirection().getZ();
  fParticleInfo.fPrimaryPosition[0] = aEvent->GetPrimaryVertex()->GetPosition().getX() / cm;
  fParticleInfo.fPrimaryPosition[1] = aEvent->GetPrimaryVertex()->GetPosition().getY() / cm;
  fParticleInfo.fPrimaryPosition[2] = aEvent->GetPrimaryVertex()->GetPosition().getZ() / cm;
  fEdep = 0;
  fPhotonCount_Ceren = 0;
  fPhotonCount_Scint = 0;
  fAbsorptionCount = 0;
  fBoundaryAbsorptionCount = 0;
  fEscapeCount = 0;
  fDetectionCount = 0;
  fWLSConvertionCount = 0;
  fWLSGenerationCount = 0;
  fPrimaryVertex = aEvent->GetPrimaryVertex();
  fPrimaryParticle = fPrimaryVertex->GetPrimary();
}

void EventAction::EndOfEventAction(const G4Event *aEvent)
{
  G4int evtNb = aEvent->GetEventID();
  if ((evtNb + 1) % 10 == 0 || fParticleInfo.fDecayChain.length() > 1)
    G4cout << " end of event " << evtNb << " :" + fParticleInfo.fDecayChain
           << G4endl;
  fParticleInfo.fEnergyDeposition = fEdep;
  fParticleInfo.fPhotonGen = fPhotonCount_Scint + fPhotonCount_Ceren;
  fParticleInfo.fPhotonWLS = fWLSGenerationCount;
  fParticleInfo.fPhotonSelfAbs = fAbsorptionCount;
  fParticleInfo.fPhotonBounAbs = fBoundaryAbsorptionCount;
  fParticleInfo.fPhotonDet = fDetectionCount;
  fHistoManager->SetParticleInfo(fParticleInfo);
  // update the run statistics
  Run *run = static_cast<Run *>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  run->IncEDep(fEdep);
  run->IncNPho(fPhotonCount_Scint + fPhotonCount_Ceren);
  run->IncNPhoScin(fPhotonCount_Scint);
  run->IncNPhoCeren(fPhotonCount_Ceren);
  run->IncSelfAbsPho(fAbsorptionCount);
  run->IncBounAbsPho(fBoundaryAbsorptionCount);
  run->IncEscPho(fEscapeCount);
  run->IncDetPho(fDetectionCount);
  run->IncWLSConPho(fWLSConvertionCount);
  run->IncWLSGenPho(fWLSGenerationCount);
}

void EventAction::Addinfo(G4double particleKinetic, G4double GlobalTime, G4double LocalTime, G4ThreeVector vpos)
{
  // cost too much memory space to store information of every single photons
  // fParticleInfo.fPhotonEnergy.push_back(particleKinetic / eV);
  // fParticleInfo.fPhotonGlobalTime.push_back(GlobalTime / ns);
  // fParticleInfo.fPhotonLocalTime.push_back(LocalTime / ns);
  // fParticleInfo.fPhotonPositionX.push_back(vpos.x() / cm);
  // fParticleInfo.fPhotonPositionY.push_back(vpos.y() / cm);
  // fParticleInfo.fPhotonPositionZ.push_back(vpos.z() / cm);
  if ((particleKinetic / eV) < 10)
    fParticleInfo.fPhotonEnergyHis[int(particleKinetic / eV / 0.01)]++;
  if ((GlobalTime / ns) < 500)
    fParticleInfo.fPhotonGlobalTimeHis[int(GlobalTime / ns / 0.1)]++;
}

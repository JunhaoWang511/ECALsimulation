#include "TrackingAction.hh"

#include "HistoManager.hh"
#include "EventAction.hh"

#include "G4Track.hh"
#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4DynamicParticle.hh"

TrackingAction::TrackingAction(EventAction *aEventAction)
    : G4UserTrackingAction(), fEventAction(aEventAction), fHistoManager() {
  fHistoManager = HistoManager::Instance();
}

TrackingAction::~TrackingAction() {}

void TrackingAction::PreUserTrackingAction(const G4Track *track) {
  G4ParticleDefinition *particle = track->GetDefinition();
  fCharge = particle->GetPDGCharge();
  fMass = particle->GetPDGMass();
  name = particle->GetParticleName();
  type = particle->GetParticleType();
  ID = track->GetTrackID();
  ParentID = track->GetParentID();
  postPhyVolume = track->GetNextVolume();
}

void TrackingAction::PostUserTrackingAction(const G4Track *) {
}

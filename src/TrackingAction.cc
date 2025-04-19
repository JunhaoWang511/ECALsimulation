#include "TrackingAction.hh"
#include "Trajectory.hh"
#include "TrackInformation.hh"
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
    : G4UserTrackingAction(), fEventAction(aEventAction)
{
}

TrackingAction::~TrackingAction() {}

void TrackingAction::PreUserTrackingAction(const G4Track *aTrack)
{
  // Use custom trajectory class
  fpTrackingManager->SetTrajectory(new Trajectory(aTrack));
  // This user track information is only relevant to the photons
  fpTrackingManager->SetUserTrackInformation(new TrackInformation);
}

void TrackingAction::PostUserTrackingAction(const G4Track *aTrack)
{
  Trajectory *trajectory = (Trajectory *)fpTrackingManager->GimmeTrajectory();
  TrackInformation *trackInformation = (TrackInformation *)aTrack->GetUserInformation();
  // Draw only the photons that hit the pmt
  if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  {

    const G4VProcess *creator = aTrack->GetCreatorProcess();
    if (creator && creator->GetProcessName() == "OpWLS")
    {
      // Mark WLS photons
      trajectory->WLS();
    }
    if (trackInformation->GetTrackStatus() == detected)
      trajectory->SetDrawTrajectory(true);
  }
  else // Draw other particles
    trajectory->SetDrawTrajectory(true);
}

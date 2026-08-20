#include "StackingAction.hh"
#include "EventAction.hh"

#include "G4OpticalPhoton.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

StackingAction::StackingAction(EventAction *ea)
    : fEventAction(ea)
{
}

StackingAction::~StackingAction() {}

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track *aTrack)
{
    // particle is optical photon
    if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    {
        G4Track *track = const_cast<G4Track *>(aTrack);
        // particle is secondary
        if (aTrack->GetParentID() > 0)
        {
            if (aTrack->GetCreatorProcess()->GetProcessName() == "Scintillation")
            {
                fEventAction->IncPhotonCount_Scint();
                // track->SetGlobalTime(0); // set global time to 0 for scintillation photons
            }
            else if (aTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov")
            {
                fEventAction->IncPhotonCount_Ceren();
                // kill Cerenkov photons at beginning
                // return fKill;
            }
            else if (aTrack->GetCreatorProcess()->GetProcessName() == "OpWLS")
            {
                // count for generated WLS photons
                fEventAction->IncWLSGen();
            }
        }
    }
    return fUrgent;
}
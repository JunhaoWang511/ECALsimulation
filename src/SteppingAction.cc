#include "SteppingAction.hh"
#include "TrackInformation.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"
#include "HistoManager.hh"
#include "TrackingAction.hh"
#include "G4ProcessManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"

SteppingAction::SteppingAction(TrackingAction *aTrackingAction)
    : G4UserSteppingAction(), fTrackingAction(aTrackingAction),
      fHistoManager()
{
}
SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step *aStep)
{
  G4OpBoundaryProcessStatus boundaryStatus = Undefined;
  static G4ThreadLocal G4OpBoundaryProcess *boundary = nullptr;
  // find the boundary process only once
  if (!boundary)
  {
    G4ProcessManager *pm =
        aStep->GetTrack()->GetDefinition()->GetProcessManager();
    G4int nprocesses = pm->GetProcessListLength();
    G4ProcessVector *pv = pm->GetProcessList();
    for (G4int i = 0; i < nprocesses; ++i)
    {
      if ((*pv)[i]->GetProcessName() == "OpBoundary")
      {
        boundary = (G4OpBoundaryProcess *)(*pv)[i];
        break;
      }
    }
  }

  G4Track *aTrack = aStep->GetTrack();
  G4double GlobalTime = aTrack->GetGlobalTime();
  G4StepPoint *postStepPoint = aStep->GetPostStepPoint();
  G4VPhysicalVolume *postPhyVolume = postStepPoint->GetPhysicalVolume();

  TrackInformation *trackInfo = (TrackInformation *)aTrack->GetUserInformation();
  // get outside of world volume
  if (!postPhyVolume)
  {
    // count for photons entering World
    if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    {
      fTrackingAction->GetEventAction()->IncEscaption();
      trackInfo->SetTrackStatusFlag(escaped);
      trackInfo->SetTrackTime(GlobalTime);
    }
    return;
  }

  G4ThreeVector vposition;
  const G4DynamicParticle *particle = aTrack->GetDynamicParticle();
  G4String particleName = particle->GetDefinition()->GetParticleName();
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (postPhyVolume->GetName().contains("Crystal"))
    fTrackingAction->GetEventAction()->AddEdep(edep);

  //  count for optical photon only
  if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  {
    // count for WLS photons
    if (postStepPoint->GetProcessDefinedStep()->GetProcessName() == "OpWLS")
    {
      fTrackingAction->GetEventAction()->IncWLS();
    }

    // count for absorbed photons
    if (postStepPoint->GetProcessDefinedStep()->GetProcessName() == "OpAbsorption")
    {
      fTrackingAction->GetEventAction()->IncAbsorption();
      trackInfo->SetTrackStatusFlag(absorbed);
      trackInfo->SetTrackTime(GlobalTime);
    }

    boundaryStatus = boundary->GetStatus();
    // make sure the step is actually at boundary
    if (postStepPoint->GetStepStatus() == fGeomBoundary)
    {
      switch (boundaryStatus)
      {
      case Absorption:
      {
        fTrackingAction->GetEventAction()->IncBoundaryAbsorption();
        trackInfo->SetTrackStatusFlag(boundaryAbsorbed);
        trackInfo->SetTrackTime(GlobalTime);
        break;
      }
      case Detection:
      {
        // Ignore Cherenkov photons
        if (aTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov")
          return;
        assert(postPhyVolume->GetName().contains("Cathode_phy") && aTrack->GetTrackStatus() == fStopAndKill);
        G4double LocalTime = aTrack->GetLocalTime();
        G4double particleKinetic = postStepPoint->GetTotalEnergy();
        vposition = postStepPoint->GetPosition();
        fTrackingAction->GetEventAction()->IncDetection();
        fTrackingAction->GetEventAction()->Addinfo(particleKinetic, GlobalTime, LocalTime, vposition);
        trackInfo->SetTrackStatusFlag(detected);
        trackInfo->SetTrackTime(GlobalTime);
        break;
      }
      case NoRINDEX:
      case FresnelReflection:
      case TotalInternalReflection:
      case LambertianReflection:
      case LobeReflection:
      case SpikeReflection:
      case BackScattering:
        break;
      default:
        break;
      }
    }
  }
}

#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "HistoManager.hh"
#include "TrackingAction.hh"
#include "G4GeneralParticleSource.hh"
#include "G4OpBoundaryProcess.hh"

class DetectorConstruction;
class HistoManager;
class TrackingAction;
class SteppingAction : public G4UserSteppingAction
{
	public:
		SteppingAction(TrackingAction*);
		virtual ~SteppingAction();
		virtual void UserSteppingAction(const G4Step*);
		TrackingAction *GetTrackingAction(){return fTrackingAction;};
		HistoManager *GetHistoManager(){return fHistoManager;};
		
	private:
		TrackingAction *fTrackingAction;
		HistoManager *fHistoManager;
};

#endif

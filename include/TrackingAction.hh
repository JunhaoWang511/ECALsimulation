#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "globals.hh"
#include "EventAction.hh"
#include "HistoManager.hh"
#include "SteppingAction.hh"
class RunAction;
class EventAction;
class ParticleInfo;

class TrackingAction : public G4UserTrackingAction 
{

	public:  
		TrackingAction(EventAction*);
		virtual ~TrackingAction();

	public:  
		virtual void  PreUserTrackingAction(const G4Track*);
		virtual void  PostUserTrackingAction(const G4Track*);
		EventAction* GetEventAction(){return fEventAction;};
		HistoManager* GetHistoManager(){return fHistoManager;};
	private:
		EventAction* fEventAction;
		HistoManager* fHistoManager;
		G4double	fMass;
		G4double	fCharge;
		G4String name;
		G4String type;
		G4int ID;
		G4int ParentID;
		G4VPhysicalVolume*postPhyVolume;

};

#endif

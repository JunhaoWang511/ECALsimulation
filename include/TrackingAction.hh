#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "globals.hh"
class EventAction;

class TrackingAction : public G4UserTrackingAction
{
public:
	TrackingAction(EventAction *);
	virtual ~TrackingAction();
	virtual void PreUserTrackingAction(const G4Track *);
	virtual void PostUserTrackingAction(const G4Track *);
	EventAction *GetEventAction() { return fEventAction; };

private:
	EventAction *fEventAction;
};

#endif

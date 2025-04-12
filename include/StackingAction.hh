#ifndef StackingAction_H
#define StackingAction_H

#include "globals.hh"
#include "G4UserStackingAction.hh"

class EventAction;

class StackingAction : public G4UserStackingAction
{
public:
	StackingAction(EventAction *);
	virtual ~StackingAction();
	virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *aTrack);

private:
	EventAction *fEventAction;
};

#endif

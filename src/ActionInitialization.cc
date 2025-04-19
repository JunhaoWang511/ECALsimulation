#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"
#include "StackingAction.hh"
#include "SteppingAction.hh"
#include "HistoManager.hh"

ActionInitialization::ActionInitialization(G4String aName)
	: G4VUserActionInitialization(), histName(aName)
{
	fHistoManager = new HistoManager(histName);
}

ActionInitialization::~ActionInitialization()
{
	delete fHistoManager;
}
void ActionInitialization::BuildForMaster() const
{
	SetUserAction(new RunAction());
}

void ActionInitialization::Build() const
{
	SetUserAction(new PrimaryGeneratorAction());

	RunAction *runAction = new RunAction();
	SetUserAction(runAction);

	EventAction *eventAction = new EventAction(runAction);
	SetUserAction(eventAction);

	StackingAction *stackingAction = new StackingAction(eventAction);
	SetUserAction(stackingAction);

	TrackingAction *trackingAction = new TrackingAction(eventAction);
	SetUserAction(trackingAction);

	SteppingAction *steppingAction = new SteppingAction(trackingAction);
	SetUserAction(steppingAction);
}

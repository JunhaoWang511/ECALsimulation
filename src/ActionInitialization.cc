#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"
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
	// master 线程注册 UI 命令，写入共享配置，各 worker 读取
	fMessenger = new PrimaryGeneratorMessenger();
}

ActionInitialization::~ActionInitialization()
{
	delete fMessenger;
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

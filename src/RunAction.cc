#include "RunAction.hh"
#include "Run.hh"
#include <HistoManager.hh>
#include <TTree.h>
#include "G4Timer.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include <iomanip>

RunAction::RunAction()
	: fRun(), G4UserRunAction(), fHistoManager()
{
	fTimer = new G4Timer;
	fHistoManager = HistoManager::Instance();
}

G4Run *RunAction::GenerateRun()
{
	fRun = new Run();
	return fRun;
}

RunAction::~RunAction()
{
}

void RunAction::BeginOfRunAction(const G4Run *aRun)
{
	fRunID = aRun->GetRunID();
	G4cout << " ### Run " << aRun->GetRunID() << " start." << G4endl;
	fTimer->Start();
}

void RunAction::EndOfRunAction(const G4Run *aRun)
{
	fTimer->Stop();

	if (isMaster)
		fRun->EndOfRun();
}

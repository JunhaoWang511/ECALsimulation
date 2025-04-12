#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4Timer.hh"
#include <HistoManager.hh>
#include <map>

class G4Run;
class Run;
class HistoManager;

class RunAction : public G4UserRunAction
{
public:
	RunAction();
	virtual ~RunAction();
	G4Run* GenerateRun() override;
	virtual void BeginOfRunAction(const G4Run *);
	virtual void EndOfRunAction(const G4Run *);
	HistoManager *GetHistoManager() { return fHistoManager; };
	G4int getRunID() { return fRunID; };

private:
    Run *fRun;
	ParticleInfo fParticleInfo;
	HistoManager *fHistoManager;
	G4int fRunID;
	G4Timer *fTimer;
};

#endif

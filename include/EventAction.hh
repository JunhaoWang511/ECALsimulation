#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "HistoManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "DetectorConstruction.hh"
#include "RunAction.hh"

class EventAction : public G4UserEventAction
{
public:
  EventAction(RunAction *);
  virtual ~EventAction();
  virtual void BeginOfEventAction(const G4Event *);
  virtual void EndOfEventAction(const G4Event *);
  RunAction *GetRunAction()
  {
    return fRunAction;
  };
  HistoManager *GetHistoManager()
  {
    return fHistoManager;
  };
  void AddDecayChain(G4String val)
  {
    fParticleInfo.fDecayChain += val;
  };
  void Addinfo(G4double, G4double, G4double, G4ThreeVector);
  void AddEdep(G4double edep) { fEdep += edep; }
  void IncPhotonCount_Scint() { fPhotonCount_Scint++; }
  void IncPhotonCount_Ceren() { fPhotonCount_Ceren++; }
  void IncAbsorption() { fAbsorptionCount++; }
  void IncBoundaryAbsorption() { fBoundaryAbsorptionCount++; }
  void IncEscaption() { fEscapeCount++; }
  void IncDetection() { fDetectionCount++; }
  void IncWLS() { fWLSCount++; }
  // Gets the total photon count produced
  G4int GetPhotonCount() { return fPhotonCount_Scint + fPhotonCount_Ceren; }
  G4int GetPhotonCount_Scint() const { return fPhotonCount_Scint; }
  G4int GetPhotonCount_Ceren() const { return fPhotonCount_Ceren; }
  G4int GetAbsorptionCount() const { return fAbsorptionCount; }
  G4int GetBoundaryAbsorptionCount() const { return fBoundaryAbsorptionCount; }
  G4int GetEscapeCount() const { return fEscapeCount; }
  G4int GetDetectionCount() const { return fDetectionCount; }
  G4int GetWLSCount() const { return fWLSCount; }

private:
  ParticleInfo fParticleInfo;
  RunAction *fRunAction;
  HistoManager *fHistoManager;
  G4double fEdep;
  G4int fPhotonCount_Scint;
  G4int fPhotonCount_Ceren;
  G4int fAbsorptionCount;
  G4int fBoundaryAbsorptionCount;
  G4int fEscapeCount;
  G4int fDetectionCount;
  G4int fWLSCount;
  G4PrimaryParticle *fPrimaryParticle;
  G4PrimaryVertex *fPrimaryVertex;
};

#endif

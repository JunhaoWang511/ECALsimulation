#ifndef Run_h
#define Run_h 1

#include "globals.hh"
#include "G4Run.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class Run : public G4Run
{
public:
    Run();
    ~Run();

    void IncEDep(G4double dep) { fTotE += dep; }
    void IncNPho(G4int pho) { fPhoGen += pho; }
    void IncNPhoScin(G4int pho) { fPhoGenScin += pho; }
    void IncNPhoCeren(G4int pho) { fPhoGenCeren += pho; }
    void IncSelfAbsPho(G4int pho) { fPhoSelfAbs += pho; }
    void IncBounAbsPho(G4int pho) { fPhoBounAbs += pho; }
    void IncEscPho(G4int pho) { fPhoEsc += pho; }
    void IncDetPho(G4int pho) { fPhoDet += pho; }
    void IncWLSConPho(G4int pho) { fPhoConWLS += pho; }
    void IncWLSGenPho(G4int pho) { fPhoGenWLS += pho; }

    void Merge(const G4Run *run) override;
    void EndOfRun();

private:
    G4double fTotE;
    G4long fPhoGen;
    G4long fPhoGenScin;
    G4long fPhoGenCeren;
    G4long fPhoConWLS;
    G4long fPhoGenWLS;
    G4long fPhoSelfAbs;
    G4long fPhoBounAbs;
    G4long fPhoEsc;
    G4long fPhoDet;
};

#endif // Run_h
